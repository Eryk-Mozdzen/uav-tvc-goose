/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "protocol.h"
#include "msg.h"
#include "nvm.h"
#include "nmea.h"
#include "mpu6050_regs.h"
#include "qmc5883l_regs.h"
#include "bmp280_regs.h"
#include "bmp280_compensate.h"
#include "ina226_regs.h"
#include "pmw3901_regs.h"
#include "ekf.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;
I2C_HandleTypeDef hi2c4;
DMA_HandleTypeDef handle_GPDMA1_Channel3;
DMA_HandleTypeDef handle_GPDMA1_Channel2;
DMA_HandleTypeDef handle_GPDMA1_Channel5;
DMA_HandleTypeDef handle_GPDMA1_Channel4;
DMA_HandleTypeDef handle_GPDMA1_Channel7;
DMA_HandleTypeDef handle_GPDMA1_Channel6;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim8;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_NodeTypeDef Node_GPDMA1_Channel0;
DMA_QListTypeDef List_GPDMA1_Channel0;
DMA_HandleTypeDef handle_GPDMA1_Channel0;
DMA_NodeTypeDef Node_GPDMA1_Channel8;
DMA_QListTypeDef List_GPDMA1_Channel8;
DMA_HandleTypeDef handle_GPDMA1_Channel8;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_ICACHE_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2C3_Init(void);
static void MX_I2C4_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM8_Init(void);
static void MX_UART5_Init(void);
static void MX_UART4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef enum {
	SENSOR_MISC_NONE,
	SENSOR_MISC_BAR,
	SENSOR_MISC_PWR_VOLTAGE,
	SENSOR_MISC_PWR_CURRENT,
	SENSOR_MISC_PWR_MASK,
} sensor_misc_t;

typedef enum {
	BUFFER_EVENT_NONE,
	BUFFER_EVENT_HALF_COMPLETE,
	BUFFER_EVENT_COMPLETE,
} buffer_event_t;

static protocol_t protocol = PROTOCOL_INIT;

static uint8_t imu_buffer[14];
static uint8_t mag_buffer[6];
static uint8_t misc_buffer[6];

static volatile bool imu_ready = false;
static volatile bool mag_ready = false;
static volatile bool misc_ready = false;
static volatile bool range_ready = false;
static volatile bool pwr_int = false;
static volatile buffer_event_t gps_event = BUFFER_EVENT_NONE;
static volatile uint32_t range_duration = 0;

static bool send_calibration = false;
static sensor_misc_t misc_busy = SENSOR_MISC_NONE;

static void qmc5883l_write(uint8_t address, uint8_t value) {
	HAL_I2C_Mem_Write(&hi2c3, QMC5883L_ADDR<<1, address, 1, &value, 1, 100);
}

static void qmc5883l_init() {
	qmc5883l_write(QMC5883L_REG_CONTROL_2,
		QMC5883L_CONFIG_2_SOFT_RST
	);

	HAL_Delay(100);

    qmc5883l_write(QMC5883L_REG_SET_RESET,
		QMC5883L_SET_RESET_RECOMMENDED
	);

    qmc5883l_write(QMC5883L_REG_CONTROL_2,
		QMC5883L_CONFIG_2_INT_ENB_ENABLE
	);

	qmc5883l_write(QMC5883L_REG_CONTROL_1,
        QMC5883L_CONFIG_1_OSR_512 |
        QMC5883L_CONFIG_1_RNG_8G |
		QMC5883L_CONFIG_1_ODR_200HZ |
        QMC5883L_CONFIG_1_MODE_CONTINOUS
	);
}

static void qmc5883l_read(float *mag, const uint8_t *buffer) {
	const int16_t raw_x = (((int16_t)buffer[1])<<8) | buffer[0];
	const int16_t raw_y = (((int16_t)buffer[3])<<8) | buffer[2];
	const int16_t raw_z = (((int16_t)buffer[5])<<8) | buffer[4];

	const float gain = 1.f/3000.f;

	mag[0] = -raw_z*gain;
	mag[1] = +raw_x*gain;
	mag[2] = -raw_y*gain;
}

static void mpu6050_write(uint8_t address, uint8_t value) {
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR<<1, address, 1, &value, 1, 100);
}

static void mpu6050_init() {
	mpu6050_write(MPU6050_REG_PWR_MGMT_1,
		MPU6050_PWR_MGMT_1_DEVICE_RESET
	);

	HAL_Delay(100);

	mpu6050_write(MPU6050_REG_SIGNAL_PATH_RESET,
		MPU6050_SIGNAL_PATH_RESET_GYRO |
		MPU6050_SIGNAL_PATH_RESET_ACCEL |
		MPU6050_SIGNAL_PATH_RESET_TEMP
	);

	HAL_Delay(100);

	mpu6050_write(MPU6050_REG_INT_ENABLE,
		MPU6050_INT_ENABLE_FIFO_OVERLOW_DISABLE |
		MPU6050_INT_ENABLE_I2C_MST_INT_DISABLE |
		MPU6050_INT_ENABLE_DATA_RDY_ENABLE
	);

	mpu6050_write(MPU6050_REG_INT_PIN_CFG,
		MPU6050_INT_PIN_CFG_LEVEL_ACTIVE_HIGH |
		MPU6050_INT_PIN_CFG_PUSH_PULL |
		MPU6050_INT_PIN_CFG_PULSE |
		MPU6050_INT_PIN_CFG_STATUS_CLEAR_AFTER_ANY |
		MPU6050_INT_PIN_CFG_FSYNC_DISABLE |
		MPU6050_INT_PIN_CFG_I2C_BYPASS_DISABLE
	);

	mpu6050_write(MPU6050_REG_PWR_MGMT_1,
		MPU6050_PWR_MGMT_1_TEMP_DIS |
		MPU6050_PWR_MGMT_1_CLOCK_INTERNAL
	);

	mpu6050_write(MPU6050_REG_CONFIG,
		MPU6050_CONFIG_EXT_SYNC_DISABLED |
		MPU6050_CONFIG_DLPF_SETTING_6
	);

	mpu6050_write(MPU6050_REG_ACCEL_CONFIG,
		MPU6050_ACCEL_CONFIG_RANGE_4G
	);

	mpu6050_write(MPU6050_REG_GYRO_CONFIG,
		MPU6050_GYRO_CONFIG_RANGE_500DPS
	);

	mpu6050_write(MPU6050_REG_SMPLRT_DIV, 0);
}

static void mpu6050_read(float *acc, float *gyr, const uint8_t *buffer) {
	{
		const int16_t raw_x = (((int16_t)buffer[8])<<8) | buffer[9];
		const int16_t raw_y = (((int16_t)buffer[10])<<8) | buffer[11];
		const int16_t raw_z = (((int16_t)buffer[12])<<8) | buffer[13];

		const float gain = 65.5f;
		const float dps_to_rads = 0.017453292519943f;

		gyr[0] = +raw_z*dps_to_rads/gain;
		gyr[1] = +raw_y*dps_to_rads/gain;
		gyr[2] = -raw_x*dps_to_rads/gain;
	}

	{
		const int16_t raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
		const int16_t raw_y = (((int16_t)buffer[2])<<8) | buffer[3];
		const int16_t raw_z = (((int16_t)buffer[4])<<8) | buffer[5];

		const float gain = 8192.f;
		const float g_to_ms2 = 9.80665f;

		acc[0] = -raw_z*g_to_ms2/gain;
		acc[1] = -raw_y*g_to_ms2/gain;
		acc[2] = +raw_x*g_to_ms2/gain;
	}
}

void bmp280_write(uint8_t address, uint8_t value) {
	HAL_I2C_Mem_Write(&hi2c4, BMP280_ADDR<<1, address, 1, &value, 1, 100);
}

void bmp280_init() {
	bmp280_write(BMP280_REG_RESET,
		BMP280_RESET_VALUE
	);

	HAL_Delay(100);

	bmp280_write(BMP280_REG_CTRL_MEAS,
		BMP280_CTRL_TEMP_OVERSAMPLING_2 |
		BMP280_CTRL_PRESS_OVERSAMPLING_16 |
		BMP280_CTRL_MODE_NORMAL
	);

	bmp280_write(BMP280_REG_CONFIG,
		BMP280_CONFIG_STANDBY_0_5MS |
		BMP280_CONFIG_FILTER_X16 |
		BMP280_CONFIG_SPI_3WIRE_DISABLE
	);

	uint8_t buffer[24] = {0};

	HAL_I2C_Mem_Read(&hi2c4, BMP280_ADDR<<1, BMP280_REG_CALIB00, 1, buffer, sizeof(buffer), 100);

	dig_T1 = (((uint16_t)buffer[1])<<8) | buffer[0];
	dig_T2 = (((int16_t)buffer[3])<<8) | buffer[2];
	dig_T3 = (((int16_t)buffer[5])<<8) | buffer[4];
	dig_P1 = (((uint16_t)buffer[7])<<8) | buffer[6];
	dig_P2 = (((int16_t)buffer[9])<<8) | buffer[8];
	dig_P3 = (((int16_t)buffer[11])<<8) | buffer[10];
	dig_P4 = (((int16_t)buffer[13])<<8) | buffer[12];
	dig_P5 = (((int16_t)buffer[15])<<8) | buffer[14];
	dig_P6 = (((int16_t)buffer[17])<<8) | buffer[16];
	dig_P7 = (((int16_t)buffer[19])<<8) | buffer[18];
	dig_P8 = (((int16_t)buffer[21])<<8) | buffer[20];
	dig_P9 = (((int16_t)buffer[23])<<8) | buffer[22];
}

void bmp280_read(float *pressure, const uint8_t *buffer) {
	const int32_t raw_temperature  = (((int32_t)buffer[3])<<12) | (((int32_t)buffer[4])<<4) | (((int32_t)buffer[5])>>4);
	const int32_t raw_pressure     = (((int32_t)buffer[0])<<12) | (((int32_t)buffer[1])<<4) | (((int32_t)buffer[2])>>4);

	float temp = bmp280_compensate_T_int32(raw_temperature)/100.f;	// *C
	*pressure  = bmp280_compensate_P_int64(raw_pressure)/256.f;		// Pa

	(void)temp;
}

#define PWR_R_SHUNT		0.002f
#define PWR_MAX_CURRENT	20.f

void ina226_write(uint8_t address, uint16_t value) {
	uint8_t reverse[2] = {
		(uint8_t)(value>>8),
		(uint8_t)(value)
	};

	HAL_I2C_Mem_Write(&hi2c4, INA226_ADDR<<1, address, 1, reverse, 2, 100);
}

void ina226_init() {
	ina226_write(INA226_REG_CONFIGURATION,
		INA226_CONFIGURATION_RESET
	);

	HAL_Delay(100);

	ina226_write(INA226_REG_CONFIGURATION,
		INA226_CONFIGURATION_AVERAGE_1 |
		INA226_CONFIGURATION_BUS_VOLTAGE_CONV_1_1MS |
		INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_1_1MS |
		INA226_CONFIGURATION_MODE_CONTINUOUS_SHUNT_BUS
	);

	ina226_write(INA226_REG_MASK_ENABLE,
		INA226_MASK_ENABLE_CONVERSION_READY |
		INA226_MASK_ENABLE_ALERT_POLARITY_ACTIVE_LOW |
		INA226_MASK_ENABLE_ALERT_LATCH_TRANSPARENT
	);

	const uint16_t calib = INA226_CALIBRATION_VALUE(PWR_MAX_CURRENT, PWR_R_SHUNT);

	ina226_write(INA226_REG_CALIBRATION, calib);
}

void ina226_read(float *power, const uint8_t *buffer) {
	{
		const int16_t raw = (((uint16_t)buffer[0])<<8) | buffer[1];
		power[0] = raw*INA226_LSB_BUS_VOLTAGE;
	}
	{
		const int16_t raw = (((uint16_t)buffer[2])<<8) | buffer[3];
		power[1] = raw*INA226_LSB_CURRENT(PWR_MAX_CURRENT);
	}
}

void pmw3901_write(const uint8_t address, const uint8_t value) {
	uint8_t tx[] = {address | 0x80, value};

	HAL_GPIO_WritePin(FLOW_CS_GPIO_Port, FLOW_CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_Transmit(&hspi1, tx, sizeof(tx), HAL_MAX_DELAY);
	HAL_Delay(1);
	HAL_GPIO_WritePin(FLOW_CS_GPIO_Port, FLOW_CS_Pin, GPIO_PIN_SET);
	HAL_Delay(1);
}

void pmw3901_read(const uint8_t address, uint8_t *buffer, const uint8_t len) {
	uint8_t tx[32] = {0};
	uint8_t rx[32] = {0};

	for(uint8_t i=0; i<len; i++) {
		tx[2*i] = (address + i) & ~0x80;
	}

	HAL_GPIO_WritePin(FLOW_CS_GPIO_Port, FLOW_CS_Pin, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_SPI_TransmitReceive(&hspi1, tx, rx, 2*len, HAL_MAX_DELAY);
	HAL_Delay(1);
	HAL_GPIO_WritePin(FLOW_CS_GPIO_Port, FLOW_CS_Pin, GPIO_PIN_SET);
	HAL_Delay(1);

	for(uint8_t i=0; i<len; i++) {
		buffer[i] = rx[2*i + 1];
	}
}

void pmw3901_init() {
	pmw3901_write(0x3A, 0x5A);

	HAL_Delay(5);

	pmw3901_write(0x7F, 0x00);
	pmw3901_write(0x61, 0xAD);
	pmw3901_write(0x7F, 0x03);
	pmw3901_write(0x40, 0x00);
	pmw3901_write(0x7F, 0x05);
	pmw3901_write(0x41, 0xB3);
	pmw3901_write(0x43, 0xF1);
	pmw3901_write(0x45, 0x14);
	pmw3901_write(0x5B, 0x32);
	pmw3901_write(0x5F, 0x34);
	pmw3901_write(0x7B, 0x08);
	pmw3901_write(0x7F, 0x06);
	pmw3901_write(0x44, 0x1B);
	pmw3901_write(0x40, 0xBF);
	pmw3901_write(0x4E, 0x3F);
	pmw3901_write(0x7F, 0x08);
	pmw3901_write(0x65, 0x20);
	pmw3901_write(0x6A, 0x18);
	pmw3901_write(0x7F, 0x09);
	pmw3901_write(0x4F, 0xAF);
	pmw3901_write(0x5F, 0x40);
	pmw3901_write(0x48, 0x80);
	pmw3901_write(0x49, 0x80);
	pmw3901_write(0x57, 0x77);
	pmw3901_write(0x60, 0x78);
	pmw3901_write(0x61, 0x78);
	pmw3901_write(0x62, 0x08);
	pmw3901_write(0x63, 0x50);
	pmw3901_write(0x7F, 0x0A);
	pmw3901_write(0x45, 0x60);
	pmw3901_write(0x7F, 0x00);
	pmw3901_write(0x4D, 0x11);
	pmw3901_write(0x55, 0x80);
	pmw3901_write(0x74, 0x1F);
	pmw3901_write(0x75, 0x1F);
	pmw3901_write(0x4A, 0x78);
	pmw3901_write(0x4B, 0x78);
	pmw3901_write(0x44, 0x08);
	pmw3901_write(0x45, 0x50);
	pmw3901_write(0x64, 0xFF);
	pmw3901_write(0x65, 0x1F);
	pmw3901_write(0x7F, 0x14);
	pmw3901_write(0x65, 0x60);
	pmw3901_write(0x66, 0x08);
	pmw3901_write(0x63, 0x78);
	pmw3901_write(0x7F, 0x15);
	pmw3901_write(0x48, 0x58);
	pmw3901_write(0x7F, 0x07);
	pmw3901_write(0x41, 0x0D);
	pmw3901_write(0x43, 0x14);
	pmw3901_write(0x4B, 0x0E);
	pmw3901_write(0x45, 0x0F);
	pmw3901_write(0x44, 0x42);
	pmw3901_write(0x4C, 0x80);
	pmw3901_write(0x7F, 0x10);
	pmw3901_write(0x5B, 0x02);
	pmw3901_write(0x7F, 0x07);
	pmw3901_write(0x40, 0x41);
	pmw3901_write(0x70, 0x00);

	HAL_Delay(100);

	pmw3901_write(0x32, 0x44);
	pmw3901_write(0x7F, 0x07);
	pmw3901_write(0x40, 0x40);
	pmw3901_write(0x7F, 0x06);
	pmw3901_write(0x62, 0xf0);
	pmw3901_write(0x63, 0x00);
	pmw3901_write(0x7F, 0x0D);
	pmw3901_write(0x48, 0xC0);
	pmw3901_write(0x6F, 0xd5);
	pmw3901_write(0x7F, 0x00);
	pmw3901_write(0x5B, 0xa0);
	pmw3901_write(0x4E, 0xA8);
	pmw3901_write(0x5A, 0x50);
	pmw3901_write(0x40, 0x80);
}

static void comm_transmit(void *user, const void *data, const uint32_t size) {
    (void)user;
    HAL_UART_Transmit_DMA(&huart4, data, size);
}

static void comm_receive(void *user, const uint8_t id, const uint32_t time, const void *payload, const uint32_t size) {
    (void)user;
    (void)time;

    switch(id) {
        case MSG_ID_CALIBRATION: {
            if(size==sizeof(msg_frame_calibration_t)) {
                nvm_write(0, payload, size);
            }

            send_calibration = true;
        } break;
    }
}

static void comm_error(void *user, const protocol_error_t error) {
    (void)user;
    (void)error;
}

static uint32_t comm_time(void *user) {
	(void)user;
	return HAL_GetTick();
}

static void logger(const char *format, ...) {
    va_list args;
    va_start(args, format);

	char buffer[256];
    const uint16_t len = vsprintf(buffer, format, args);

    protocol_enqueue(&protocol, MSG_ID_LOG, buffer, len);
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin==IMU_INT_Pin) {
		HAL_I2C_Mem_Read_DMA(&hi2c1, MPU6050_ADDR<<1, MPU6050_REG_ACCEL_XOUT_H, 1, imu_buffer, sizeof(imu_buffer));
	} else if(GPIO_Pin==MAG_INT_Pin) {
		HAL_I2C_Mem_Read_DMA(&hi2c3, QMC5883L_ADDR<<1, QMC5883L_REG_DATA_OUTPUT_X_LSB, 1, mag_buffer, sizeof(mag_buffer));
	} else if(GPIO_Pin==RANGE_ECHO_Pin) {
		range_duration = __HAL_TIM_GET_COUNTER(&htim8);
	}
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin==PWR_INT_Pin) {
		pwr_int = true;
	} else if(GPIO_Pin==RANGE_ECHO_Pin) {
		range_duration = __HAL_TIM_GET_COUNTER(&htim8) - range_duration;
		range_ready = true;
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if(hi2c==&hi2c1) {
		imu_ready = true;
	} else if(hi2c==&hi2c3) {
		mag_ready = true;
	} else if(hi2c==&hi2c4) {
		misc_ready = true;
	}
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	if(huart==&huart5) {
		gps_event = BUFFER_EVENT_HALF_COMPLETE;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart==&huart5) {
		gps_event = BUFFER_EVENT_COMPLETE;
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the System Power */
  SystemPower_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  MX_I2C1_Init();
  MX_I2C3_Init();
  MX_I2C4_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM8_Init();
  MX_UART5_Init();
  MX_UART4_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint8_t buffer_tx[10*1024];
  uint8_t buffer_rx[10*1024];
  uint8_t buffer_decode[10*1024];

  protocol.callback_tx = comm_transmit;
  protocol.callback_rx = comm_receive;
  protocol.callback_err = comm_error;
  protocol.callback_time = comm_time;
  protocol.fifo_tx.buffer = buffer_tx;
  protocol.fifo_tx.size = sizeof(buffer_tx);
  protocol.fifo_rx.buffer = buffer_rx;
  protocol.fifo_rx.size = sizeof(buffer_rx);
  protocol.decoded = buffer_decode;
  protocol.max = sizeof(buffer_decode);

  HAL_UART_Receive_DMA(&huart4, protocol.fifo_rx.buffer, protocol.fifo_rx.size);

  uint8_t gps_buffer[16];
  HAL_UART_Receive_DMA(&huart5, gps_buffer, sizeof(gps_buffer));

  HAL_TIM_Base_Start(&htim8);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);

  logger("system reset");

  qmc5883l_init();
  mpu6050_init();
  bmp280_init();
  ina226_init();
  pmw3901_init();

  uint32_t last_blink = 0;
  uint32_t last_barometer = 0;
  uint32_t last_flow = 0;
  uint32_t last_sensor = 0;
  uint32_t last_estimation = 0;
  uint32_t last_controller = 0;

  msg_frame_sensor_t sensor = {0};
  msg_frame_estimation_t estimation = {0};
  msg_frame_controller_t controller = {0};
  msg_frame_calibration_t calibration = {0};
  nmea_messaage_t nmea_message = {0};

  while(1) {
	  const uint32_t time = HAL_GetTick();

	  protocol.fifo_rx.write = protocol.fifo_rx.size - __HAL_DMA_GET_COUNTER(huart4.hdmarx);
	  protocol.available = (HAL_DMA_GetState(huart4.hdmatx)==HAL_DMA_STATE_READY);
	  protocol_process(&protocol);

	  if(imu_ready) {
		  imu_ready = false;
		  mpu6050_read(sensor.accelerometer.raw, sensor.gyroscope.raw, imu_buffer);

		  nvm_read(0, &calibration, sizeof(calibration));
		  sensor.accelerometer.calib[0] = calibration.accelerometer[0]*sensor.accelerometer.raw[0] + calibration.accelerometer[1]*sensor.accelerometer.raw[1] + calibration.accelerometer[2]*sensor.accelerometer.raw[2] + calibration.accelerometer[9];
		  sensor.accelerometer.calib[1] = calibration.accelerometer[3]*sensor.accelerometer.raw[0] + calibration.accelerometer[4]*sensor.accelerometer.raw[1] + calibration.accelerometer[5]*sensor.accelerometer.raw[2] + calibration.accelerometer[10];
		  sensor.accelerometer.calib[2] = calibration.accelerometer[6]*sensor.accelerometer.raw[0] + calibration.accelerometer[7]*sensor.accelerometer.raw[1] + calibration.accelerometer[8]*sensor.accelerometer.raw[2] + calibration.accelerometer[11];
		  sensor.gyroscope.calib[0] = sensor.gyroscope.raw[0] + calibration.gyroscope[0];
		  sensor.gyroscope.calib[1] = sensor.gyroscope.raw[1] + calibration.gyroscope[1];
		  sensor.gyroscope.calib[2] = sensor.gyroscope.raw[2] + calibration.gyroscope[2];

		  sensor.valid.accelerometer = 1;
		  sensor.valid.gyroscope = 1;
	  }

	  if(mag_ready) {
		  mag_ready = false;
		  qmc5883l_read(sensor.magnetometer.raw, mag_buffer);

		  nvm_read(0, &calibration, sizeof(calibration));
		  sensor.magnetometer.calib[0] = calibration.magnetometer[0]*sensor.magnetometer.raw[0] + calibration.magnetometer[1]*sensor.magnetometer.raw[1] + calibration.magnetometer[2]*sensor.magnetometer.raw[2] + calibration.magnetometer[9];
		  sensor.magnetometer.calib[1] = calibration.magnetometer[3]*sensor.magnetometer.raw[0] + calibration.magnetometer[4]*sensor.magnetometer.raw[1] + calibration.magnetometer[5]*sensor.magnetometer.raw[2] + calibration.magnetometer[10];
		  sensor.magnetometer.calib[2] = calibration.magnetometer[6]*sensor.magnetometer.raw[0] + calibration.magnetometer[7]*sensor.magnetometer.raw[1] + calibration.magnetometer[8]*sensor.magnetometer.raw[2] + calibration.magnetometer[11];

		  sensor.valid.magnetometer = 1;
	  }

	  if((time - last_barometer)>=50 && misc_busy==SENSOR_MISC_NONE) {
		  last_barometer = time;
		  misc_ready = false;
		  misc_busy = SENSOR_MISC_BAR;
		  HAL_I2C_Mem_Read_DMA(&hi2c4, BMP280_ADDR<<1, BMP280_REG_PRESS_MSB, 1, misc_buffer, 6);
	  }

	  if(pwr_int && misc_busy==SENSOR_MISC_NONE) {
		  pwr_int = false;
		  misc_ready = false;
		  misc_busy = SENSOR_MISC_PWR_VOLTAGE;
		  HAL_I2C_Mem_Read_DMA(&hi2c4, INA226_ADDR<<1, INA226_REG_BUS_VOLTAGE, 1, &misc_buffer[0], 2);
	  }

	  if(misc_ready) {
		  misc_ready = false;

		  switch(misc_busy) {
			  case SENSOR_MISC_NONE: {

			  } break;
			  case SENSOR_MISC_BAR: {
				  misc_busy = SENSOR_MISC_NONE;
				  bmp280_read(&sensor.barometer, misc_buffer);
				  sensor.valid.barometer = 1;
			  } break;
			  case SENSOR_MISC_PWR_VOLTAGE: {
				  misc_busy = SENSOR_MISC_PWR_CURRENT;
				  HAL_I2C_Mem_Read_DMA(&hi2c4, INA226_ADDR<<1, INA226_REG_CURRENT, 1, &misc_buffer[2], 2);
			  } break;
			  case SENSOR_MISC_PWR_CURRENT: {
				  misc_busy = SENSOR_MISC_PWR_MASK;
				  HAL_I2C_Mem_Read_DMA(&hi2c4, INA226_ADDR<<1, INA226_REG_MASK_ENABLE, 1, &misc_buffer[4], 2);
			  } break;
			  case SENSOR_MISC_PWR_MASK: {
				  misc_busy = SENSOR_MISC_NONE;
				  ina226_read(sensor.power, misc_buffer);
				  sensor.valid.power = 1;
			  } break;
		  }
	  }

	  if(gps_event!=BUFFER_EVENT_NONE) {
		  const uint8_t size = sizeof(gps_buffer)/2;
		  const uint8_t *src = (gps_event==BUFFER_EVENT_HALF_COMPLETE) ? gps_buffer : &gps_buffer[size];
		  gps_event = BUFFER_EVENT_NONE;

		  protocol_enqueue(&protocol, MSG_ID_PASSTHROUGH_GPS, src, size);

		  for(size_t s=0; s<size; s++) {
			  const char c = src[s];

			  if(nmea_consume(&nmea_message, c)) {
				  if(strcmp(nmea_message.argv[0], "GPRMC")==0 && strcmp(nmea_message.argv[2], "A")==0) {
					  float latitude = 0;
					  float longitude = 0;

					  {
						  const float minutes = atof(&nmea_message.argv[3][2]);
						  nmea_message.argv[3][2] = '\0';
						  const int degree = atoi(nmea_message.argv[3]);
						  latitude = degree + minutes/60.f;
					  }

					  {
						  const float minutes = atof(&nmea_message.argv[5][3]);
						  nmea_message.argv[5][3] = '\0';
						  const int degree = atoi(nmea_message.argv[5]);
						  longitude = degree + minutes/60.f;
					  }

					  sensor.gps[0] = latitude;
					  sensor.gps[1] = longitude;
					  sensor.valid.gps = 1;
				  }
			  }
		  }
	  }

	  if(range_ready) {
		  range_ready = false;
		  const float speed_of_sound = 343.f;
		  const float duration = range_duration*0.00001f;
		  const float range = duration*speed_of_sound/2.f;
		  if(range<2.f) {
			  sensor.rangefinder = range;
			  sensor.valid.rangefinder = 1;
		  }
	  }

	  if((time - last_flow)>=20) {
		  const float dt = (time - last_flow)*0.001f;

		  last_flow = time;

		  uint8_t motion[5] = {0};
		  pmw3901_read(PMW3901_REG_MOTION, motion, sizeof(motion));
		  const int16_t delta_x = (((int16_t)motion[2])<<8) | motion[1];
		  const int16_t delta_y = (((int16_t)motion[4])<<8) | motion[3];

		  sensor.flow[0] = delta_x/(dt*PMW3901_FOCAL_LENGTH);
		  sensor.flow[1] = delta_y/(dt*PMW3901_FOCAL_LENGTH);
		  sensor.valid.flow = 1;
	  }

	  if((time - last_blink)>=500) {
		  last_blink = time;
		  HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
	  }

	  if((time - last_sensor)>=100) {
		  last_sensor = time;
		  protocol_enqueue(&protocol, MSG_ID_SENSOR, &sensor, sizeof(sensor));
		  sensor.valid_all = 0;
	  }

	  if((time - last_estimation)>=20) {
		  last_estimation = time;
		  protocol_enqueue(&protocol, MSG_ID_ESTIMATION, &estimation, sizeof(estimation));
	  }

	  if((time - last_controller)>=20) {
		  last_controller = time;
		  protocol_enqueue(&protocol, MSG_ID_CONTROLLER, &controller, sizeof(controller));
	  }

	  if(send_calibration) {
		  send_calibration = false;
		  nvm_read(0, &calibration, sizeof(calibration));
		  protocol_enqueue(&protocol, MSG_ID_CALIBRATION, &calibration, sizeof(calibration));
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * PWR Privilege Configuration
   */
  HAL_PWR_ConfigAttributes(PWR_ALL, PWR_NSEC_PRIV);
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel2_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel3_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel4_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel5_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel5_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel6_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel7_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel8_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel8_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00F07BFF;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00F07BFF;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief I2C4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C4_Init(void)
{

  /* USER CODE BEGIN I2C4_Init 0 */

  /* USER CODE END I2C4_Init 0 */

  /* USER CODE BEGIN I2C4_Init 1 */

  /* USER CODE END I2C4_Init 1 */
  hi2c4.Instance = I2C4;
  hi2c4.Init.Timing = 0x00F07BFF;
  hi2c4.Init.OwnAddress1 = 0;
  hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c4.Init.OwnAddress2 = 0;
  hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C4_Init 2 */

  /* USER CODE END I2C4_Init 2 */

}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = {0};

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
  HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
  if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 1600-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 9999;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 230400;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief UART5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART5_Init(void)
{

  /* USER CODE BEGIN UART5_Init 0 */

  /* USER CODE END UART5_Init 0 */

  /* USER CODE BEGIN UART5_Init 1 */

  /* USER CODE END UART5_Init 1 */
  huart5.Instance = UART5;
  huart5.Init.BaudRate = 9600;
  huart5.Init.WordLength = UART_WORDLENGTH_8B;
  huart5.Init.StopBits = UART_STOPBITS_1;
  huart5.Init.Parity = UART_PARITY_NONE;
  huart5.Init.Mode = UART_MODE_RX;
  huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart5, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart5, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART5_Init 2 */

  /* USER CODE END UART5_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, LOAD_RATE_Pin|LOAD_SCK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLOW_CS_GPIO_Port, FLOW_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_G_Pin|LED_B_Pin|LED_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LOAD_RATE_Pin LOAD_SCK_Pin */
  GPIO_InitStruct.Pin = LOAD_RATE_Pin|LOAD_SCK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LOAD_DOUT_Pin */
  GPIO_InitStruct.Pin = LOAD_DOUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LOAD_DOUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MAG_INT_Pin FLOW_INT_Pin */
  GPIO_InitStruct.Pin = MAG_INT_Pin|FLOW_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : FLOW_CS_Pin */
  GPIO_InitStruct.Pin = FLOW_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(FLOW_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_G_Pin LED_B_Pin LED_R_Pin */
  GPIO_InitStruct.Pin = LED_G_Pin|LED_B_Pin|LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : RANGE_ECHO_Pin */
  GPIO_InitStruct.Pin = RANGE_ECHO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RANGE_ECHO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : IMU_INT_Pin */
  GPIO_InitStruct.Pin = IMU_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PWR_INT_Pin */
  GPIO_InitStruct.Pin = PWR_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(PWR_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

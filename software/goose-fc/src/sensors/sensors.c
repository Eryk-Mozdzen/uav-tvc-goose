#include "sensors.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "sensor_bus.h"
#include "hmc5883l.h"
#include "bmp280.h"
#include "logger.h"
#include "queue_element.h"

#define IMU_ADDRESS		0x68

QueueHandle_t sensor_queue;

TaskHandle_t imuReaderTask;
TaskHandle_t magReaderTask;

extern bmp280_calib_data_t bmp280_calib_data;

void imu_reader(void *param) {
	(void)param;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	SensorBus_WriteReg(IMU_ADDRESS, 0x68, 0x07);		// reset
	SensorBus_WriteReg(IMU_ADDRESS, 0x38, 0x01);		// data ready interrupt enable
	SensorBus_WriteReg(IMU_ADDRESS, 0x6B, 0x00);		
	SensorBus_WriteReg(IMU_ADDRESS, 0x1C, 0x10);
	SensorBus_WriteReg(IMU_ADDRESS, 0x1B, 0x10);

	LOG(LOG_INFO, "IMU initialized\n\r");

	uint8_t buffer[14];

	queue_element_t reading;

	while(1) {
		ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		if(SensorBus_ReadRegs(IMU_ADDRESS, 0x3B, buffer, sizeof(buffer))) {
			continue;
		}

		const int16_t acc_raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
		const int16_t acc_raw_y = (((int16_t)buffer[2])<<8) | buffer[3];
		const int16_t acc_raw_z = (((int16_t)buffer[4])<<8) | buffer[5];

		const float3_t acc = {
			.x = acc_raw_x,
			.y = acc_raw_y,
			.z = acc_raw_z
		};

		reading.type = SENSOR_ACCELEROMETER;
		reading.data = pvPortMalloc(sizeof(float3_t));
		memcpy(reading.data, &acc, sizeof(float3_t));
		xQueueSend(sensor_queue, &reading, 0);

		const int16_t gyr_raw_x = (((int16_t)buffer[8])<<8) | buffer[9];
		const int16_t gyr_raw_y = (((int16_t)buffer[10])<<8) | buffer[11];
		const int16_t gyr_raw_z = (((int16_t)buffer[12])<<8) | buffer[13];

		const float3_t gyr = {
			.x = gyr_raw_x,
			.y = gyr_raw_y,
			.z = gyr_raw_z
		};

		reading.type = SENSOR_GYROSCOPE;
		reading.data = pvPortMalloc(sizeof(float3_t));
		memcpy(reading.data, &gyr, sizeof(float3_t));
		xQueueSend(sensor_queue, &reading, 0);
	}
}

void mag_reader(void *param) {
	(void)param;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	SensorBus_WriteReg(HMC5883L_ADDR, HMC5883L_REG_CONFIG_A, 
		HMC5883L_CONFIG_A_MEAS_NORMAL |
		HMC5883L_CONFIG_A_RATE_75 |
		HMC5883L_CONFIG_A_SAMPLES_8
	);
	SensorBus_WriteReg(HMC5883L_ADDR, HMC5883L_REG_CONFIG_B, 
		HMC5883L_CONFIG_B_RANGE_1_3GA
	);
	SensorBus_WriteReg(HMC5883L_ADDR, HMC5883L_REG_MODE, 
		HMC5883L_MODE_CONTINOUS
	);

	LOG(LOG_INFO, "MAG initialized\n\r");

	uint8_t buffer[6];

	queue_element_t reading;

	while(1) {
		ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		if(SensorBus_ReadRegs(HMC5883L_ADDR, HMC5883L_REG_DATA_X_MSB, buffer, sizeof(buffer))) {
			continue;
		}

		const int16_t raw_x = (((int16_t)buffer[0])<<8) | buffer[1];
		const int16_t raw_y = (((int16_t)buffer[4])<<8) | buffer[5];
		const int16_t raw_z = (((int16_t)buffer[2])<<8) | buffer[3];

		if((raw_x>2047 || raw_x<-2048) || (raw_y>2047 || raw_y<-2048) || (raw_z>2047 || raw_z<-2048)) {
			LOG(LOG_WARNING, "mag: value out of valid range [%10d %10d %10d]\n\r", raw_x, raw_y, raw_z);
		}

		const float gain = 1090.f;

		const float3_t mag = {
			.x = (raw_x +  87)/gain,
			.y = (raw_y + 143)/gain,
			.z = (raw_z -   0)/gain
		};

		reading.type = SENSOR_MAGNETOMETER;
		reading.data = pvPortMalloc(sizeof(float3_t));
		memcpy(reading.data, &mag, sizeof(float3_t));
		xQueueSend(sensor_queue, &reading, 0);
	}
}

void bar_reader(void *param) {
	(void)param;

	SensorBus_WriteReg(BMP280_ADDR, BMP280_REG_RESET, BMP280_RESET_VALUE);
	SensorBus_WriteReg(BMP280_ADDR, BMP280_REG_CTRL_MEAS, 
		BMP280_CTRL_TEMP_OVERSAMPLING_2 |
		BMP280_CTRL_PRESS_OVERSAMPLING_16 |
		BMP280_CTRL_MODE_NORMAL
	);
	SensorBus_WriteReg(BMP280_ADDR, BMP280_REG_CONFIG,
		BMP280_CONFIG_STANDBY_0_5MS |
		BMP280_CONFIG_FILTER_OFF |
		BMP280_CONFIG_SPI_3WIRE_DISABLE
	);

	{
		uint8_t calib[24] = {0};

		if(!SensorBus_ReadRegs(BMP280_ADDR, BMP280_REG_CALIB00, calib, sizeof(calib))) {
			
			bmp280_calib_data.dig_T1 = (((uint16_t)calib[1])<<8) | calib[0];
			bmp280_calib_data.dig_T2 = (((int16_t)calib[3])<<8) | calib[2];
			bmp280_calib_data.dig_T3 = (((int16_t)calib[5])<<8) | calib[4];

			bmp280_calib_data.dig_P1 = (((uint16_t)calib[7])<<8) | calib[6];
			bmp280_calib_data.dig_P2 = (((int16_t)calib[9])<<8) | calib[8];
			bmp280_calib_data.dig_P3 = (((int16_t)calib[11])<<8) | calib[10];
			bmp280_calib_data.dig_P4 = (((int16_t)calib[13])<<8) | calib[12];
			bmp280_calib_data.dig_P5 = (((int16_t)calib[15])<<8) | calib[14];
			bmp280_calib_data.dig_P6 = (((int16_t)calib[17])<<8) | calib[16];
			bmp280_calib_data.dig_P7 = (((int16_t)calib[19])<<8) | calib[18];
			bmp280_calib_data.dig_P8 = (((int16_t)calib[21])<<8) | calib[20];
			bmp280_calib_data.dig_P9 = (((int16_t)calib[23])<<8) | calib[22];

			LOG(LOG_INFO, "BAR succesfully read calibration values\n\r");
		} else {
			LOG(LOG_ERROR, "BAR error in calibration values\n\r");
		}
	}

	LOG(LOG_INFO, "BAR initialized\n\r");

	TickType_t time = xTaskGetTickCount();

	uint8_t buffer[6];

	queue_element_t reading;

	while(1) {
		vTaskDelayUntil(&time, 100);
		
		if(SensorBus_ReadRegs(BMP280_ADDR, BMP280_REG_PRESS_MSB, buffer, sizeof(buffer))) {
			continue;
		}

		const int32_t temp_raw  = (((int32_t)buffer[3])<<12) | (((int32_t)buffer[4])<<4) | (((int32_t)buffer[5])>>4);
		const int32_t press_raw = (((int32_t)buffer[0])<<12) | (((int32_t)buffer[1])<<4) | (((int32_t)buffer[2])>>4);

		const float temp  = bmp280_compensate_T_int32(temp_raw)/100.f;		// *C
		const float press = bmp280_compensate_P_int64(press_raw)/256.f;		// Pa

		reading.type = SENSOR_BAROMETER;
		reading.data = pvPortMalloc(sizeof(float));
		memcpy(reading.data, &press, sizeof(float));
		xQueueSend(sensor_queue, &reading, 0);
	}
}

void Sensors_Init() {

	sensor_queue = xQueueCreate(16, sizeof(queue_element_t));

	SensorBus_Init();

	//xTaskCreate(imu_reader, "IMU reader", 256, NULL, 4, &imuReaderTask);
	xTaskCreate(mag_reader, "MAG reader", 256, NULL, 4, &magReaderTask);
	xTaskCreate(bar_reader, "BAR reader", 256, NULL, 4, NULL);
}

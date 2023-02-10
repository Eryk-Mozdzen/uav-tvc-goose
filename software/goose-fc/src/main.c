#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_hal.h"
#include "usbd_def.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef hUsbDeviceFS;
I2C_HandleTypeDef hi2c1;

void Init() {

	HAL_Init();

	RCC_OscInitTypeDef oscillator = {
		.OscillatorType = RCC_OSCILLATORTYPE_HSE,
		.HSEState = RCC_HSE_ON,
		.PLL.PLLState = RCC_PLL_ON,
		.PLL.PLLSource = RCC_PLLSOURCE_HSE,
		.PLL.PLLM = 4,
		.PLL.PLLN = 96,
		.PLL.PLLP = RCC_PLLP_DIV2,
		.PLL.PLLQ = 4
	};

	RCC_ClkInitTypeDef clock = {
		.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV2,
		.APB2CLKDivider = RCC_HCLK_DIV1
	};

	HAL_RCC_OscConfig(&oscillator);
	HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_3);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance==TIM11) {
		HAL_IncTick();
	}
}

void blink(void *param) {
	(void)param;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio = {
		.Pin = GPIO_PIN_10,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOB, &gpio);

	while(1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
		vTaskDelay(500);
	}
}

void telemetry(void *param) {
	(void)param;

	#define MPU6050_DEVICE_ID			0x68 ///< The correct MPU6050_WHO_AM_I value
	#define MPU6050_SELF_TEST_X			0x0D ///< Self test factory calibrated values register
	#define MPU6050_SELF_TEST_Y			0x0E ///< Self test factory calibrated values register
	#define MPU6050_SELF_TEST_Z			0x0F ///< Self test factory calibrated values register
	#define MPU6050_SELF_TEST_A			0x10 ///< Self test factory calibrated values register
	#define MPU6050_SMPLRT_DIV			0x19  ///< sample rate divisor register
	#define MPU6050_CONFIG				0x1A      ///< General configuration register
	#define MPU6050_GYRO_CONFIG			0x1B ///< Gyro specfic configuration register
	#define MPU6050_ACCEL_CONFIG    	0x1C ///< Accelerometer specific configration register
	#define MPU6050_INT_PIN_CONFIG		0x37 ///< Interrupt pin configuration register
	#define MPU6050_INT_ENABLE			0x38     ///< Interrupt enable configuration register
	#define MPU6050_INT_STATUS			0x3A     ///< Interrupt status register
	#define MPU6050_WHO_AM_I			0x75       ///< Divice ID register
	#define MPU6050_SIGNAL_PATH_RESET	0x68 ///< Signal path reset register
	#define MPU6050_USER_CTRL			0x6A         ///< FIFO and I2C Master control register
	#define MPU6050_PWR_MGMT_1			0x6B        ///< Primary power/sleep control register
	#define MPU6050_PWR_MGMT_2			0x6C ///< Secondary power/sleep control register
	#define MPU6050_TEMP_H				0x41     ///< Temperature data high byte register
	#define MPU6050_TEMP_L				0x42     ///< Temperature data low byte register
	#define MPU6050_ACCEL_OUT			0x3B  ///< base address for sensor data reads
	#define MPU6050_MOT_THR				0x1F    ///< Motion detection threshold bits [7:0]
	#define MPU6050_MOT_DUR				0x20 ///< Duration counter threshold for motion int. 1 kHz rate, LSB = 1 ms

	char buffer[128];

	uint8_t reg = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x6B, 1, &reg, 1, 100);
	reg = 0x10;
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x1C, 1, &reg, 1, 100);
	reg = 0x10;
	HAL_I2C_Mem_Write(&hi2c1, 0x68<<1, 0x1B, 1, &reg, 1, 100);

	reg = 0x70;
	HAL_I2C_Mem_Write(&hi2c1, 0x1E<<1, 0x00, 1, &reg, 1, 100);
	reg = 0xE0;
	HAL_I2C_Mem_Write(&hi2c1, 0x1E<<1, 0x01, 1, &reg, 1, 100);
	reg = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, 0x1E<<1, 0x02, 1, &reg, 1, 100);

	reg = 0xB6;
	HAL_I2C_Mem_Write(&hi2c1, 0x77<<1, 0xE0, 1, &reg, 1, 100);
	reg = 0xFF;
	HAL_I2C_Mem_Write(&hi2c1, 0x77<<1, 0xF4, 1, &reg, 1, 100);

	while(1) {

		uint8_t imu_data[14] = {0};
		HAL_StatusTypeDef status_imu = HAL_I2C_Mem_Read(&hi2c1, 0x68<<1, 0x3B, 1, imu_data, 14, 100);

		uint8_t mag_data[6] = {0};
		HAL_StatusTypeDef status_mag = HAL_I2C_Mem_Read(&hi2c1, 0x1E<<1, 0x03, 1, mag_data, 6, 100);

		uint8_t bar_data[6] = {0};
		HAL_StatusTypeDef status_bar = HAL_I2C_Mem_Read(&hi2c1, 0x77<<1, 0xF7, 1, bar_data, 6, 100);

		if(status_imu==HAL_OK && status_mag==HAL_OK && status_bar==HAL_OK) {
			int16_t acc[3] = {0};
			int16_t gyr[3] = {0};
			int16_t mag[3] = {0};
			int32_t bar = 0;

			acc[0] = (((int16_t)imu_data[0])<<8) | imu_data[1];
			acc[1] = (((int16_t)imu_data[2])<<8) | imu_data[3];
			acc[2] = (((int16_t)imu_data[4])<<8) | imu_data[5];
			gyr[0] = (((int16_t)imu_data[8])<<8) | imu_data[9];
			gyr[1] = (((int16_t)imu_data[10])<<8) | imu_data[11];
			gyr[2] = (((int16_t)imu_data[12])<<8) | imu_data[13];

			mag[0] = (((int16_t)mag_data[0])<<8) | mag_data[1];
			mag[1] = (((int16_t)mag_data[4])<<8) | mag_data[5];
			mag[2] = (((int16_t)mag_data[2])<<8) | mag_data[3];

			bar = (((int32_t)bar_data[0])<<12) | (((int32_t)bar_data[1])<<4) | (((int32_t)bar_data[2])>>4);

			snprintf(buffer, sizeof(buffer), 
				"acc: %7d %7d %7d\n\rgyr: %7d %7d %7d\n\rmag: %7d %7d %7d\n\rbar: %7ld\n\r\n\r", 
				acc[0], acc[1], acc[2], 
				gyr[0], gyr[1], gyr[2],
				mag[0], mag[1], mag[2],
				bar
			);

			//snprintf(buffer, sizeof(buffer), "%02X %02X %02X %02X %02X %02X\n\r\n\r", mag_data[0], mag_data[1], mag_data[2], mag_data[3], mag_data[4], mag_data[5]);

		} else {
			snprintf(buffer, sizeof(buffer), "Error %d %d %d\n\r", status_imu, status_mag, status_bar);
		}

		CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer));

		vTaskDelay(100);
	}
}

int main() {

	Init();

	USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS);
	USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC);
	USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS);
	USBD_Start(&hUsbDeviceFS);

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hi2c1);

	xTaskCreate(blink, "blink", 1024, NULL, 4, NULL);
	xTaskCreate(telemetry, "usb", 1024, NULL, 4, NULL);

	vTaskStartScheduler();

	return 0;
}

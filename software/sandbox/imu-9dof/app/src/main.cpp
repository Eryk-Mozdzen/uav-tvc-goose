#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "lsm303dlhc.h"
#include "l3gd20.h"
#include "imu9dof.h"

TIM_HandleTypeDef htim11;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {

	HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, TickPriority, 0);
	HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);
	
	__HAL_RCC_TIM11_CLK_ENABLE();
	
	const uint32_t uwTimclock = HAL_RCC_GetPCLK2Freq();
	const uint32_t uwPrescalerValue = (uwTimclock/1000000) - 1;

	htim11.Instance = TIM11;
	htim11.Init.Period = 999;
	htim11.Init.Prescaler = uwPrescalerValue;
	htim11.Init.ClockDivision = 0;
	htim11.Init.CounterMode = TIM_COUNTERMODE_UP;

	if(HAL_TIM_Base_Init(&htim11)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_TIM_Base_Start_IT(&htim11);
}

extern "C" {

void TIM1_TRG_COM_TIM11_IRQHandler() {
	HAL_TIM_IRQHandler(&htim11);
}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance==TIM11) {
		HAL_IncTick();
	}
}

void blink(void *param) {
	(void)param;

	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef led;
	led.Pin = GPIO_PIN_5;
	led.Mode = GPIO_MODE_OUTPUT_PP;
	led.Pull = GPIO_NOPULL;
	led.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOA, &led);

	while(1) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		vTaskDelay(1000);

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		vTaskDelay(1000);
	}
}

void serial(void *param) {
	(void)param;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();

	GPIO_InitTypeDef pins;
	pins.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	pins.Mode = GPIO_MODE_AF_PP;
	pins.Pull = GPIO_NOPULL;
	pins.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	pins.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &pins);

	UART_HandleTypeDef huart2;
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.Mode = UART_MODE_TX;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_I2C3_CLK_ENABLE();

	GPIO_InitTypeDef scl;
	scl.Pin = GPIO_PIN_8;
	scl.Mode = GPIO_MODE_AF_OD;
	scl.Pull = GPIO_NOPULL;
	scl.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	scl.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOA, &scl);

	GPIO_InitTypeDef sda;
	sda.Pin = GPIO_PIN_9;
	sda.Mode = GPIO_MODE_AF_OD;
	sda.Pull = GPIO_NOPULL;
	sda.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	sda.Alternate = GPIO_AF4_I2C3;
	HAL_GPIO_Init(GPIOC, &sda);

	I2C_HandleTypeDef hi2c3;
	hi2c3.Instance = I2C3;
	hi2c3.Init.ClockSpeed = 100000;
	hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c3.Init.OwnAddress1 = 0;
	hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c3.Init.OwnAddress2 = 0;
	hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hi2c3);
	
	char buffer[128];

	IMU_9DoF imu(hi2c3);
	imu.initializeSensors();
	imu.startSampling();

	while(1) {

		/*uint8_t raw[6];
		HAL_I2C_Mem_Read(&hi2c3, LSM303DLHC::mag_address, 0x03 | 0x80, 1, raw, 6, HAL_MAX_DELAY);
		snprintf(buffer, sizeof(buffer), "[%02X %02X] [%02X %02X] [%02X %02X]\n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5]);*/
		//snprintf(buffer, sizeof(buffer), "%+-9d %+-9d %+-9d\n", (int)(raw[0]<<8 | raw[1]), (int)(raw[2]<<8 | raw[3]), (int)(raw[4]<<8 | raw[5]));

		//const IMU_9DoF::Vector3i acc = imu.getRawAcceleration();
		//const IMU_9DoF::Vector3i gyr = imu.getRawGyration();
		//const IMU_9DoF::Vector3i mag = imu.getRawMagneticField();

		const IMU_9DoF::Vector3f acc = imu.getAcceleration();
		const IMU_9DoF::Vector3f gyr = imu.getGyration();
		const IMU_9DoF::Vector3f mag = imu.getMagneticField();

		/*const float acc_len = sqrt(acc.x*acc.x + acc.y*acc.y + acc.z*acc.z);
		const float mag_len = sqrt(mag.x*mag.x + mag.y*mag.y + mag.z*mag.z);

		snprintf(buffer, sizeof(buffer), "\nACC %+-9.2f %+-9.2f %+-9.2f %+-9.2f\nGYR %+-9.2f %+-9.2f %+-9.2f\nMAG %+-9.2f %+-9.2f %+-9.2f %+-9.2f\n", 
			(double)acc.x, (double)acc.y, (double)acc.z, (double)acc_len, 
			(double)gyr.x, (double)gyr.y, (double)gyr.z, 
			(double)mag.x, (double)mag.y, (double)mag.z, (double)mag_len
		);*/

		snprintf(buffer, sizeof(buffer), "\nACC %+-9.2f %+-9.2f %+-9.2f\nGYR %+-9.2f %+-9.2f %+-9.2f\nMAG %+-9.2f %+-9.2f %+-9.2f\n", 
			(double)acc.x, (double)acc.y, (double)acc.z, 
			(double)gyr.x, (double)gyr.y, (double)gyr.z, 
			(double)mag.x, (double)mag.y, (double)mag.z
		);

		/*snprintf(buffer, sizeof(buffer), "\nACC %+-9d %+-9d %+-9d\nGYR %+-9d %+-9d %+-9d\nMAG %+-9d %+-9d %+-9d\n", 
			(int)acc.x, (int)acc.y, (int)acc.z, 
			(int)gyr.x, (int)gyr.y, (int)gyr.z, 
			(int)mag.x, (int)mag.y, (int)mag.z
		);*/

		HAL_UART_Transmit(&huart2, reinterpret_cast<uint8_t *>(buffer), strlen(buffer), HAL_MAX_DELAY);
		vTaskDelay(100);
	}
}

int main() {

	HAL_Init();

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitTypeDef oscillator;
	oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	oscillator.HSIState = RCC_HSI_ON;
	oscillator.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	oscillator.PLL.PLLState = RCC_PLL_ON;
	oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	oscillator.PLL.PLLM = 8;
	oscillator.PLL.PLLN = 100;
	oscillator.PLL.PLLP = RCC_PLLP_DIV2;
	oscillator.PLL.PLLQ = 4;

	RCC_ClkInitTypeDef clock;
	clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clock.APB1CLKDivider = RCC_HCLK_DIV2;
	clock.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_OscConfig(&oscillator);
	HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_3);

	xTaskCreate(blink, "blink", 1024, NULL, 4, NULL);
	xTaskCreate(serial, "serial", 1024, NULL, 4, NULL);

	vTaskStartScheduler();

	return 0;
}

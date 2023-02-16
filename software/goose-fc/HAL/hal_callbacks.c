#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t sensorTaskToNotify;
extern TaskHandle_t imuReaderTask;
extern TaskHandle_t magReaderTask;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance==TIM11) {
		HAL_IncTick();
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if(hi2c->Instance==I2C1) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(sensorTaskToNotify, 0, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin==GPIO_PIN_15) {

		// IMU
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(imuReaderTask, 1, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	} else if(GPIO_Pin==GPIO_PIN_5) {

		// magnetometer
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(magReaderTask, 1, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}
}

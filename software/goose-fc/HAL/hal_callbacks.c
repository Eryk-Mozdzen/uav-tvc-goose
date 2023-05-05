#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

extern TaskHandle_t task_to_notify;
extern TaskHandle_t imu_task;
extern TaskHandle_t mag_task;
extern TaskHandle_t vlx_task;
extern TaskHandle_t com_bus_task_tx;
extern TaskHandle_t com_bus_task_rx;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance==TIM11) {
		HAL_IncTick();
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if(hi2c->Instance==I2C1) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(task_to_notify, 0, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if(GPIO_Pin==GPIO_PIN_15) {

		// IMU
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(imu_task, 1, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	} else if(GPIO_Pin==GPIO_PIN_5) {

		// magnetometer
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(mag_task, 1, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	} else if(GPIO_Pin==GPIO_PIN_14) {

		// distance
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(vlx_task, 1, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart->Instance==USART1) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(com_bus_task_tx, 2, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if(huart->Instance==USART1) {
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveIndexedFromISR(com_bus_task_rx, 3, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

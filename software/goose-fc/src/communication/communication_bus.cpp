#include "communication_bus.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "logger.h"

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;
TaskHandle_t com_bus_task_tx;
TaskHandle_t com_bus_task_rx;

CommunicationBus::CommunicationBus() : lock_tx{"com bus tx lock"}, lock_rx{"com bus rx lock"} {

}

void CommunicationBus::init() {
	__HAL_RCC_DMA2_CLK_ENABLE();

	HAL_NVIC_SetPriority(DMA2_Stream7_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream7_IRQn);

	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);

	Logger::getInstance().log(Logger::INFO, "cbus: initialization complete");

	lock_tx.give();
	lock_rx.give();
}

void CommunicationBus::write(const uint8_t *src, const uint32_t len) {
	lock_tx.take(portMAX_DELAY);

	com_bus_task_tx = xTaskGetCurrentTaskHandle();

	taskENTER_CRITICAL();
	HAL_UART_Transmit_DMA(&huart1, src, len);
	taskEXIT_CRITICAL();

	if(!ulTaskNotifyTakeIndexed(2, pdTRUE, 100)) {
		Logger::getInstance().log(Logger::ERROR, "cbus: tx timeout");
	}

	lock_tx.give();
}

void CommunicationBus::read(uint8_t *dest, const uint32_t len) {
	lock_rx.take(portMAX_DELAY);

	com_bus_task_rx = xTaskGetCurrentTaskHandle();

	taskENTER_CRITICAL();
	HAL_UART_Receive_DMA(&huart1, dest, len);
	taskEXIT_CRITICAL();

	ulTaskNotifyTakeIndexed(3, pdTRUE, portMAX_DELAY);

	lock_rx.give();
}

CommunicationBus & CommunicationBus::getInstance() {
	static CommunicationBus bus;

	return bus;
}

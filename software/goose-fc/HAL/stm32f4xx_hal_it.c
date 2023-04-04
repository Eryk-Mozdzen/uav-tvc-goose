#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim11;
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern I2C_HandleTypeDef hi2c1;
extern DMA_HandleTypeDef hdma_i2c1_rx;
extern UART_HandleTypeDef huart1;
extern DMA_HandleTypeDef hdma_usart1_tx;
extern DMA_HandleTypeDef hdma_usart1_rx;

void TIM1_TRG_COM_TIM11_IRQHandler() {
	HAL_TIM_IRQHandler(&htim11);
}

void OTG_FS_IRQHandler() {
	HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}

void I2C1_EV_IRQHandler() {
	HAL_I2C_EV_IRQHandler(&hi2c1);
}

void I2C1_ER_IRQHandler() {
	HAL_I2C_ER_IRQHandler(&hi2c1);
}

void DMA1_Stream0_IRQHandler() {
	HAL_DMA_IRQHandler(&hdma_i2c1_rx);
}

void USART1_IRQHandler() {
	HAL_UART_IRQHandler(&huart1);
}

void DMA2_Stream2_IRQHandler() {
	HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

void DMA2_Stream7_IRQHandler() {
	HAL_DMA_IRQHandler(&hdma_usart1_tx);
}

void EXTI9_5_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

void EXTI15_10_IRQHandler() {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
}

#include "stm32f4xx_hal.h"

extern DMA_HandleTypeDef hdma_i2c1_rx;

void HAL_MspInit() {
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if(hi2c->Instance==I2C1) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_I2C1_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		hdma_i2c1_rx.Instance = DMA1_Stream0;
		hdma_i2c1_rx.Init.Channel = DMA_CHANNEL_1;
		hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
		hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
		hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
		hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		HAL_DMA_Init(&hdma_i2c1_rx);

		__HAL_LINKDMA(hi2c, hdmarx, hdma_i2c1_rx);

		HAL_NVIC_SetPriority(I2C1_EV_IRQn, 6, 0);
		HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
		HAL_NVIC_SetPriority(I2C1_ER_IRQn, 6, 0);
		HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
	}
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c) {
	if(hi2c->Instance==I2C1) {
		__HAL_RCC_I2C1_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);

		HAL_DMA_DeInit(hi2c->hdmarx);

		HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
		HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
	}
}

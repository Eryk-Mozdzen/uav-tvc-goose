#include "stm32f4xx_hal.h"

extern DMA_HandleTypeDef hdma_adc1;

void HAL_MspInit() {
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
}

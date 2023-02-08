#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim11;

void TIM1_TRG_COM_TIM11_IRQHandler() {
	HAL_TIM_IRQHandler(&htim11);
}

#include "stm32f4xx_hal.h"

static TIM_HandleTypeDef htim5;

void runtime_stats_init() {

	const uint32_t uwTimclock = HAL_RCC_GetPCLK2Freq();
	const uint32_t uwPrescalerValue = (uwTimclock/10000) - 1;

	__HAL_RCC_TIM5_CLK_ENABLE();

	htim5.Instance = TIM5;
	htim5.Init.Period = 4294967295;
	htim5.Init.Prescaler = uwPrescalerValue;
	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&htim5);

	HAL_TIM_Base_Start(&htim5);
}

uint32_t runtime_stats_get() {
    return __HAL_TIM_GET_COUNTER(&htim5);
}

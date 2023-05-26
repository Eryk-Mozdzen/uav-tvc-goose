#include "stm32f4xx_hal.h"

static TIM_HandleTypeDef htim2;

void runtime_stats_init() {

	const uint32_t uwTimclock = HAL_RCC_GetPCLK2Freq();
	const uint32_t uwPrescalerValue = (uwTimclock/10000) - 1;

	__HAL_RCC_TIM2_CLK_ENABLE();

	htim2.Instance = TIM2;
	htim2.Init.Period = 4294967295;
	htim2.Init.Prescaler = uwPrescalerValue;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&htim2);

	HAL_TIM_Base_Start(&htim2);
}

uint32_t runtime_stats_get() {
    return __HAL_TIM_GET_COUNTER(&htim2);
}

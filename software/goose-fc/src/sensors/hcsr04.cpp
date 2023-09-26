#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"
#include "interval_logger.h"

#include "stm32f4xx_hal.h"

class HCSR04 : TaskClassS<512> {
	static constexpr float speed_of_sound = 340.f;

	IntervalLogger<float> telemetry;

	void init();

public:
	HCSR04();

	float getDistance() const;

	void task();
};

TaskHandle_t dist_task;
uint32_t dist_time;
TIM_HandleTypeDef htim10;

HCSR04 ultrasonic;

HCSR04::HCSR04() : TaskClassS{"HC-SR04 reader", TaskPrio_Low},
		telemetry{"HC-SR04 telemetry", Transfer::ID::SENSOR_DISTANCE} {

}

void HCSR04::init() {
	GPIO_InitTypeDef GPIO_InitStruct;

	// echo
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	// trigger
	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

	htim10.Instance = TIM10;
	htim10.Init.Period = 49999;
	htim10.Init.Prescaler = 95;
	htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
	HAL_TIM_Base_Init(&htim10);

	__HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, 10);

	HAL_TIM_Base_Start_IT(&htim10);
	HAL_TIM_PWM_Start_IT(&htim10, TIM_CHANNEL_1);
}

float HCSR04::getDistance() const {
	const float time = dist_time*0.000001f;

	return speed_of_sound*time/2.f;
}

void HCSR04::task() {
	dist_task = getTaskHandle();

	init();

	while(1) {
		ulTaskNotifyTakeIndexed(1, pdTRUE, portMAX_DELAY);

		const float dist = getDistance();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::LASER, dist, 0);

		telemetry.feed(dist);
	}
}

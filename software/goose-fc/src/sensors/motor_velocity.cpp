#include "TaskCPP.h"

#include <cmath>

#include "logger.h"
#include "transport.h"
#include "interval_logger.h"
#include "moving_average.h"

#include "stm32f4xx_hal.h"

class MotorVelocity : TaskClassS<512> {
	IntervalLogger<float> telemetry;

	static constexpr float motor_pole_pairs = 7.f;
	static constexpr float frequency = 50.f;
	static constexpr float delta_time = 1.f/frequency;
	static constexpr float pi = 3.1415f;
	static constexpr float average_time = 1.f;

	MovingAverage<static_cast<uint32_t>(average_time*frequency)> average;

	TIM_HandleTypeDef htim2;

	void init();

public:
	MotorVelocity();

	void task();
};

MotorVelocity motor_velocity;

MotorVelocity::MotorVelocity() : TaskClassS{"Motor Velocity reader", TaskPrio_Low},
		telemetry{"Motor Velocity telemetry", Transfer::ID::SENSOR_MOTOR_VELOCITY} {

}

void MotorVelocity::init() {

	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4294967295;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	HAL_TIM_Base_Init(&htim2);

	TIM_ClockConfigTypeDef sClockSourceConfig;
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
	sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
	sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
	sClockSourceConfig.ClockFilter = 0;
  	HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

	TIM_MasterConfigTypeDef sMasterConfig;
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

	HAL_TIM_Base_Start(&htim2);
}

void MotorVelocity::task() {

	init();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, delta_time*1000);

		const float counter = __HAL_TIM_GET_COUNTER(&htim2);
		__HAL_TIM_SET_COUNTER(&htim2, 0);

		const float rps = counter/motor_pole_pairs;
		const float current_velocity = 2.f*pi*rps;

		average.add(current_velocity);

		const float filtererd_velocity = average.get();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::HALL, filtererd_velocity, 0);

		telemetry.feed(filtererd_velocity);
	}
}

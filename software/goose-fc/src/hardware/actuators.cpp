#include "actuators.h"
#include <cmath>

void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim) {
	GPIO_InitTypeDef GPIO_InitStruct;

	if(htim->Instance==TIM1) {
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	} else if(htim->Instance==TIM3) {
		__HAL_RCC_GPIOB_CLK_ENABLE();
		__HAL_RCC_GPIOC_CLK_ENABLE();

		GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}
}

Actuators::Actuators() {

}

void Actuators::init() {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

	htim3_servo.Instance = TIM3;
	htim3_servo.Init.Period = arr - 1;
	htim3_servo.Init.Prescaler = psc - 1;
	htim3_servo.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3_servo.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_PWM_Init(&htim3_servo);

    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3_servo, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&htim3_servo, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&htim3_servo, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&htim3_servo, &sConfigOC, TIM_CHANNEL_4);

    HAL_TIM_MspPostInit(&htim3_servo);

    HAL_TIM_PWM_Start(&htim3_servo, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3_servo, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3_servo, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3_servo, TIM_CHANNEL_4);

	htim1_esc.Instance = TIM1;
	htim1_esc.Init.Period = arr - 1;
	htim1_esc.Init.Prescaler = psc - 1;
	htim1_esc.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1_esc.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_PWM_Init(&htim1_esc);
    HAL_TIM_PWM_ConfigChannel(&htim1_esc, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_MspPostInit(&htim1_esc);
    HAL_TIM_PWM_Start(&htim1_esc, TIM_CHANNEL_3);
}

void Actuators::setFinAngle(const Fin fin, float alpha) {
    alpha = alpha>max_servo ? max_servo : alpha<-max_servo ? -max_servo : alpha;

    __HAL_TIM_SET_COMPARE(&htim3_servo, fin, Servo::center_compare + Servo::radius_compare*alpha/(0.5f*pi));
}

void Actuators::setMotorThrottle(float thorttle) {
    thorttle = thorttle>1.f ? 1.f : thorttle<0.f ? 0.f : thorttle;

    __HAL_TIM_SET_COMPARE(&htim1_esc, TIM_CHANNEL_3, ESC::min_compare + (ESC::max_compare - ESC::min_compare)*thorttle);
}

float Actuators::getFinAngle(const Fin fin) const {
    const float compare = __HAL_TIM_GET_COMPARE(&htim3_servo, fin);

    return (compare - ((float)Servo::center_compare))/((float)Servo::radius_compare)*0.5f*pi;
}

float Actuators::getMotorThrottle() const {
    const uint16_t compare = __HAL_TIM_GET_COMPARE(&htim1_esc, TIM_CHANNEL_3);

    return (((float)compare) - ((float)ESC::min_compare))/(((float)ESC::max_compare) - ((float)ESC::min_compare));
}

Actuators & Actuators::getInstance() {
    static Actuators instance;

    return instance;
}

#include "lights.h"

void Lights::init() {
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gpio);
}

void Lights::set(const Light light, const bool state) {
	HAL_GPIO_WritePin(GPIOB, light, static_cast<GPIO_PinState>(state));
}

void Lights::toggle(const Light light) {
	HAL_GPIO_TogglePin(GPIOB, light);
}

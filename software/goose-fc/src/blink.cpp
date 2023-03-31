#include "stm32f4xx_hal.h"
#include "TaskCPP.h"

class Blink : public TaskClassS<512> {
public:
	Blink();
	void task();
};

Blink blink;

Blink::Blink() : TaskClassS{"blink", TaskPrio_Idle} {

}

void Blink::task() {
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_10;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &gpio);

	while(1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
		vTaskDelay(500);
	}
}

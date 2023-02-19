#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "logger.h"
#include "sensors.h"
#include "communication.h"
#include "state_estimator.h"

void Init() {

	HAL_Init();

	RCC_OscInitTypeDef oscillator = {
		.OscillatorType = RCC_OSCILLATORTYPE_HSE,
		.HSEState = RCC_HSE_ON,
		.PLL.PLLState = RCC_PLL_ON,
		.PLL.PLLSource = RCC_PLLSOURCE_HSE,
		.PLL.PLLM = 4,
		.PLL.PLLN = 96,
		.PLL.PLLP = RCC_PLLP_DIV2,
		.PLL.PLLQ = 4
	};

	RCC_ClkInitTypeDef clock = {
		.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2,
		.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK,
		.AHBCLKDivider = RCC_SYSCLK_DIV1,
		.APB1CLKDivider = RCC_HCLK_DIV2,
		.APB2CLKDivider = RCC_HCLK_DIV1
	};

	HAL_RCC_OscConfig(&oscillator);
	HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_3);
}

void blink(void *param) {
	(void)param;

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio = {
		.Pin = GPIO_PIN_10,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	};
	HAL_GPIO_Init(GPIOB, &gpio);

	while(1) {
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_10);
		vTaskDelay(500);
	}
}

/*void print(void *param) {
	(void)param;

	state_t state;

	while(1) {
		xQueueReceive(state_queue, &state, portMAX_DELAY);

		LOG(LOG_INFO, "RPY: %+7.2f %+7.2f %+7.2f m/s2\n\r", (double)state.attitude.x, (double)state.attitude.y, (double)state.attitude.z);
	}
}*/

int main() {

	Init();

	Communication_Init();
	Sensors_Init();
	StateEstimator_Init();

	xTaskCreate(blink, "blink", 1024, NULL, 1, NULL);
	//xTaskCreate(blink, "print", 1024, NULL, 4, NULL);

	vTaskStartScheduler();

	return 0;
}

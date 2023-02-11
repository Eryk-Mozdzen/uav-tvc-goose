#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "usb.h"
#include "sensors.h"

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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if(htim->Instance==TIM11) {
		HAL_IncTick();
	}
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

void telemetry(void *param) {
	(void)param;

	float3_t acc;
	float3_t gyr;
	float3_t mag;
	float bar;

	while(1) {
		xQueueReceive(acc_queue, &acc, portMAX_DELAY);
		xQueueReceive(gyr_queue, &gyr, portMAX_DELAY);
		xQueueReceive(mag_queue, &mag, portMAX_DELAY);
		xQueueReceive(bar_queue, &bar, portMAX_DELAY);

		USB_print("acc: %+10.2f %+10.2f %+10.2f\n\rgyr: %+10.2f %+10.2f %+10.2f\n\rmag: %+10.2f %+10.2f %+10.2f\n\rbar: %+10.2f\n\r\n\r", 
			(double)acc.x, (double)acc.y, (double)acc.z, 
			(double)gyr.x, (double)gyr.y, (double)gyr.z,
			(double)mag.x, (double)mag.y, (double)mag.z,
			(double)bar
		);
	}
}

int main() {

	Init();

	USB_Init();
	Sensors_Init();

	xTaskCreate(blink, "blink", 1024, NULL, 4, NULL);
	xTaskCreate(telemetry, "telemetry", 1024, NULL, 4, NULL);

	vTaskStartScheduler();

	return 0;
}

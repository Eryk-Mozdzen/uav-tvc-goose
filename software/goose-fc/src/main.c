#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "logger.h"
#include "sensors.h"
#include "communication.h"

#include "queue_element.h"

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

void print(void *param) {
	(void)param;

	queue_element_t reading;

	while(1) {
		xQueueReceive(sensor_queue, &reading, portMAX_DELAY);

		switch(reading.type) {
			case SENSOR_ACCELEROMETER: {
				float3_t acc;
				memcpy(&acc, reading.data, sizeof(float3_t));
				LOG(LOG_INFO, "acc: %+10.2f %+10.2f %+10.2f m/s2\n\r", (double)acc.x, (double)acc.y, (double)acc.z);
			} break;
			case SENSOR_GYROSCOPE: {
				float3_t gyr;
				memcpy(&gyr, reading.data, sizeof(float3_t));
				LOG(LOG_INFO, "gyr: %+10.2f %+10.2f %+10.2f rad/s\n\r", (double)gyr.x, (double)gyr.y, (double)gyr.z);
			} break;
			case SENSOR_MAGNETOMETER: {
				float3_t mag;
				memcpy(&mag, reading.data, sizeof(float3_t));
				LOG(LOG_INFO, "mag: %+10.2f %+10.2f %+10.2f Ga\n\r", (double)mag.x, (double)mag.y, (double)mag.z);
			} break;
			case SENSOR_BAROMETER: {
				float bar;
				memcpy(&bar, reading.data, sizeof(float));
				LOG(LOG_INFO, "bar: %+10.2f Pa\n\r", (double)bar);
			} break;
			default: {
				LOG(LOG_WARNING, "unknown\n\r");
			} break;
		}

		vPortFree(reading.data);
	}
}

int main() {

	Init();

	Communication_Init();
	Sensors_Init();

	xTaskCreate(blink, "blink", 1024, NULL, 1, NULL);
	xTaskCreate(print, "print", 1024, NULL, 4, NULL);

	vTaskStartScheduler();

	return 0;
}

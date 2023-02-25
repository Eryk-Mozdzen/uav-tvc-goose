#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "mpu6050.h"
#include "bmp280.h"
#include "hmc5883l.h"
#include "sensor_bus.h"
#include "communication.h"
#include "state_estimator.h"
#include "queue_element.h"

TaskHandle_t transmitter_task;
TaskHandle_t imu_task;
TaskHandle_t mag_task;

void Init() {

	HAL_Init();

	RCC_OscInitTypeDef oscillator;
	oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscillator.HSEState = RCC_HSE_ON;
	oscillator.PLL.PLLState = RCC_PLL_ON;
	oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscillator.PLL.PLLM = 4;
	oscillator.PLL.PLLN = 96;
	oscillator.PLL.PLLP = RCC_PLLP_DIV2;
	oscillator.PLL.PLLQ = 4;

	RCC_ClkInitTypeDef clock;
	clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clock.APB1CLKDivider = RCC_HCLK_DIV2;
	clock.APB2CLKDivider = RCC_HCLK_DIV1;

	HAL_RCC_OscConfig(&oscillator);
	HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_3);

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

void blink(void *param) {
	(void)param;

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

int main() {

	Init();

	COM.log(Communication::INFO, "hard reset\n\r");

	SensorBus::getInstance().init();
	
	QueueHandle_t sensor_queue = xQueueCreate(16, sizeof(queue_element_t));

	xTaskCreate(transmitterTaskFcn,	"TX",			512,	NULL,		  3, &transmitter_task);
	xTaskCreate(estimatorTaskFcn,	"estimator",	1024,	sensor_queue, 4, NULL);
	xTaskCreate(imuTaskFcn,			"IMU reader",	512,	sensor_queue, 2, &imu_task);
	xTaskCreate(magTaskFcn,			"MAG reader",	512,	sensor_queue, 2, &mag_task);
	xTaskCreate(barTaskFcn,			"BAR reader",	512,	sensor_queue, 2, NULL);

	xTaskCreate(blink, "blink", 1024, NULL, 1, NULL);

	vTaskStartScheduler();

	return 0;
}

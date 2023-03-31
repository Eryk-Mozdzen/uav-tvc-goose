#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"

#include "logger.h"
#include "sensor_bus.h"
#include "communication_bus.h"

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

int main() {

	Init();

	Logger::getInstance().log(Logger::INFO, "hardware reset");

	SensorBus::getInstance().init();
	CommunicationBus::getInstance().init();

	vTaskStartScheduler();
}

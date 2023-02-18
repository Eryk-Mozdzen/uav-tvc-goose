#include "sensor_bus.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "logger.h"

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;

TaskHandle_t sensorTaskToNotify;
static SemaphoreHandle_t bus_lock;

static void slave_recovery() {
	// config I2C SDA and SCL pin as IO pins
	// manualy toggle SCL line to generate clock pulses until 10 consecutive 1 on SDA occure

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

	LOG(LOG_INFO, "bus: performing recovery from slaves...\n\r");

	int ones = 0;
	while(ones<=10) {
		if(!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)) {
			ones = 0;
		}

		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
		HAL_Delay(10);

		ones++;
	}
}

void SensorBus_Init() {
	bus_lock = xSemaphoreCreateBinary();

	slave_recovery();

	__HAL_RCC_DMA1_CLK_ENABLE();

	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 6, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	HAL_I2C_Init(&hi2c1);

	xSemaphoreGive(bus_lock);

	LOG(LOG_INFO, "bus: initialization complete\n\r");
}

void SensorBus_WriteReg(const uint8_t device, const uint8_t reg, uint8_t src) {
	xSemaphoreTake(bus_lock, portMAX_DELAY);

	taskENTER_CRITICAL();
	const HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, device << 1, reg, 1, &src, 1, 100);
	taskEXIT_CRITICAL();

	switch(status) {
		case HAL_ERROR:		LOG(LOG_ERROR, "bus: write into 0x%02X device ended with HAL_ERROR\n\r", device);	break;
		case HAL_BUSY:		LOG(LOG_ERROR, "bus: write into 0x%02X device ended with HAL_BUSY\n\r", device);	break;
		case HAL_TIMEOUT:	LOG(LOG_ERROR, "bus: write into 0x%02X device ended with HAL_TIMEOUT\n\r", device);	break;
	}

	xSemaphoreGive(bus_lock);
}

uint8_t SensorBus_ReadRegs(const uint8_t device, const uint8_t reg, uint8_t *dest, const uint8_t len) {
	xSemaphoreTake(bus_lock, portMAX_DELAY);

	sensorTaskToNotify = xTaskGetCurrentTaskHandle();

	taskENTER_CRITICAL();
	const HAL_StatusTypeDef status = HAL_I2C_Mem_Read_DMA(&hi2c1, device << 1, reg, 1, dest, len);
	taskEXIT_CRITICAL();

	if(status!=HAL_OK) {
		switch(status) {
			case HAL_ERROR:		LOG(LOG_ERROR, "bus: read from 0x%02X device ended with HAL_ERROR\n\r", device);	break;
			case HAL_BUSY:		LOG(LOG_ERROR, "bus: read from 0x%02X device ended with HAL_BUSY\n\r", device);		break;
			case HAL_TIMEOUT:	LOG(LOG_ERROR, "bus: read from 0x%02X device ended with HAL_TIMEOUT\n\r", device);	break;
		}

		xSemaphoreGive(bus_lock);
		return 1;
	}

	if(!ulTaskNotifyTakeIndexed(0, pdTRUE, 100)) {
		LOG(LOG_ERROR, "bus: timeout while reading from 0x%02X device\n\r", device);
		xSemaphoreGive(bus_lock);
		return 2;
	}

	xSemaphoreGive(bus_lock);
	return 0;
}

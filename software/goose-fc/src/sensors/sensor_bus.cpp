#include "sensor_bus.h"

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

#include "logger.h"

I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
TaskHandle_t task_to_notify;

SensorBus::SensorBus() : lock{"sensor bus lock"} {

}

void SensorBus::init() {
	slaveRecovery();

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

	lock.give();

	Logger::getInstance().log(Logger::INFO, "bus: initialization complete");
}

void SensorBus::slaveRecovery() {
	// config I2C SDA and SCL pin as IO pins
	// manualy toggle SCL line to generate clock pulses until 10 consecutive 1 on SDA occure

	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_8;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

	Logger::getInstance().log(Logger::INFO, "bus: performing recovery from slaves...");

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

void SensorBus::write(const uint8_t device, const uint8_t reg, uint8_t src) {
	lock.take(portMAX_DELAY);

	const HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, device<<1, reg, 1, &src, 1, 100);

	switch(status) {
		case HAL_ERROR:		Logger::getInstance().log(Logger::ERROR, "bus: write into 0x%02X device ended with HAL_ERROR", device);		break;
		case HAL_BUSY:		Logger::getInstance().log(Logger::ERROR, "bus: write into 0x%02X device ended with HAL_BUSY", device);		break;
		case HAL_TIMEOUT:	Logger::getInstance().log(Logger::ERROR, "bus: write into 0x%02X device ended with HAL_TIMEOUT", device);	break;
		case HAL_OK: break;
	}

	lock.give();
}

int SensorBus::read(const uint8_t device, const uint8_t reg, uint8_t *dest, const uint8_t len) {
	lock.take(portMAX_DELAY);

	task_to_notify = xTaskGetCurrentTaskHandle();

	const HAL_StatusTypeDef status = HAL_I2C_Mem_Read_DMA(&hi2c1, device<<1, reg, 1, dest, len);

	if(status!=HAL_OK) {
		switch(status) {
			case HAL_ERROR:		Logger::getInstance().log(Logger::ERROR, "bus: read from 0x%02X device ended with HAL_ERROR", device);		break;
			case HAL_BUSY:		Logger::getInstance().log(Logger::ERROR, "bus: read from 0x%02X device ended with HAL_BUSY", device);		break;
			case HAL_TIMEOUT:	Logger::getInstance().log(Logger::ERROR, "bus: read from 0x%02X device ended with HAL_TIMEOUT", device);	break;
			case HAL_OK: break;
		}

		lock.give();
		return 1;
	}

	if(!ulTaskNotifyTakeIndexed(0, pdTRUE, 100)) {
		Logger::getInstance().log(Logger::ERROR, "bus: timeout while reading from 0x%02X device", device);
		lock.give();
		return 2;
	}

	lock.give();
	return 0;
}

SensorBus& SensorBus::getInstance() {
	static SensorBus bus;

	return bus;
}

#pragma once

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

class SensorBus {
		SemaphoreHandle_t lock;

		SensorBus();
		void slaveRecovery();

	public:

		SensorBus(SensorBus &) = delete;
		void operator=(const SensorBus &) = delete;

		void init();

		void write(const uint8_t device, const uint8_t reg, uint8_t src);
		int read(const uint8_t device, const uint8_t reg, uint8_t *dest, const uint8_t len);

		static SensorBus& getInstance();
};

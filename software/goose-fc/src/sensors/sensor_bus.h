#pragma once

#include "MutexCPP.h"

class SensorBus {
		Mutex lock;

		SensorBus();
		void slaveRecovery();

	public:

		SensorBus(SensorBus &) = delete;
		void operator=(const SensorBus &) = delete;

		void init();

		void write(const uint8_t device, const uint8_t reg, uint8_t src);
		void write(const uint8_t device, const uint8_t reg, uint16_t src);
		int read(const uint8_t device, const uint8_t reg, uint8_t *dest, const uint8_t len);

		static SensorBus& getInstance();
};

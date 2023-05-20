#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"

#include "stm32f4xx_hal.h"

class PowerStats : TaskClassS<512> {

	struct Analog {
		static constexpr float reference = 3.3f;
		static constexpr uint8_t resolution = 12;
		static constexpr float adc2volt = reference/(1<<resolution);
	};

	struct SupplyVoltage {
		static constexpr float R11 = 470000.f;
		static constexpr float R12 = 100000.f;
		static constexpr float gain = R11/(R11+R12);
	};

	struct SupplyCurrent {
		static constexpr float R33 = 4700.f;
		static constexpr float R34 = 210000.f;
		static constexpr float Rshunt = 0.001f;
		static constexpr float gain = (R34/R33)*Rshunt;
	};

	ADC_HandleTypeDef hadc1;

	uint16_t readings[2];

	void gpioInit();
	void adcInit();

public:
	PowerStats();

	float getSupplyVoltage() const;
	float getSupplyCurrent() const;

	void task();
};

PowerStats power_stats;

PowerStats::PowerStats() : TaskClassS{"Power Stats reader", TaskPrio_Low} {

}

void PowerStats::gpioInit() {

}

void PowerStats::adcInit() {

}

float PowerStats::getSupplyVoltage() const {
	const float voltage = readings[0]*Analog::adc2volt;

	return voltage/SupplyVoltage::gain;
}

float PowerStats::getSupplyCurrent() const {
	const float voltage = readings[1]*Analog::adc2volt;

	return voltage/SupplyCurrent::gain;
}

void PowerStats::task() {
	gpioInit();
	adcInit();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 20);

		const float voltage = getSupplyVoltage();
		const float current = getSupplyCurrent();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::VOLTAGE, voltage, 0);
		Transport::getInstance().sensor_queue.add(Transport::Sensors::CURRENT, current, 0);
	}
}

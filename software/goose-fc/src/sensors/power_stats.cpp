#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"
#include "interval_logger.h"

class PowerStats : TaskClassS<512> {
	IntervalLogger<float> telemetry_voltage;
	IntervalLogger<float> telemetry_current;

public:
	PowerStats();

	float getSupplyVoltage() const;
	float getSupplyCurrent() const;

	void task();
};

PowerStats power_stats;

PowerStats::PowerStats() : TaskClassS{"Power Stats reader", TaskPrio_Low},
		telemetry_voltage{"Power Stats telemetry voltage", Transfer::ID::SENSOR_VOLTAGE},
		telemetry_current{"Power Stats telemetry current", Transfer::ID::SENSOR_CURRENT} {

}

float PowerStats::getSupplyVoltage() const {
	return 0;
}

float PowerStats::getSupplyCurrent() const {
	return 0;
}

void PowerStats::task() {

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 20);

		const float voltage = getSupplyVoltage();
		const float current = getSupplyCurrent();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::VOLTAGE, voltage, 0);
		Transport::getInstance().sensor_queue.add(Transport::Sensors::CURRENT, current, 0);

		telemetry_voltage.feed(voltage);
		telemetry_current.feed(current);
	}
}

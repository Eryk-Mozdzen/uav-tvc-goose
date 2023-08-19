#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"
#include "sensor_bus.h"
#include "interval_logger.h"

#define INA226_ADDR					0x40

#define INA226_REG_CONFIGURATION	0x00
#define INA226_REG_SHUNT_VOLTAGE	0x01
#define INA226_REG_BUS_VOLTAGE		0x02
#define INA226_REG_POWER			0x03
#define INA226_REG_CURRENT			0x04
#define INA226_REG_CALIBRATION		0x05
#define INA226_REG_MASK_ENABLE		0x06
#define INA226_REG_ALERT_LIMIT		0x07
#define INA226_REG_MANUFACTURER_ID	0xFE
#define INA226_REG_DIE_ID			0xFF

#define INA226_CONFIGURATION_RESET						(0x01<<15)
#define INA226_CONFIGURATION_AVERAGE_1					(0x00<<9)
#define INA226_CONFIGURATION_AVERAGE_4					(0x01<<9)
#define INA226_CONFIGURATION_AVERAGE_16					(0x02<<9)
#define INA226_CONFIGURATION_AVERAGE_64					(0x03<<9)
#define INA226_CONFIGURATION_AVERAGE_128				(0x04<<9)
#define INA226_CONFIGURATION_AVERAGE_256				(0x05<<9)
#define INA226_CONFIGURATION_AVERAGE_512				(0x06<<9)
#define INA226_CONFIGURATION_AVERAGE_1024				(0x07<<9)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_140US		(0x00<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_204US		(0x01<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_332US		(0x02<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_588US		(0x03<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_1_1MS		(0x04<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_2_116MS	(0x05<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_4_156MS	(0x06<<6)
#define INA226_CONFIGURATION_BUS_VOLTAGE_CONV_8_244MS	(0x07<<6)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_140US	(0x00<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_204US	(0x01<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_332US	(0x02<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_588US	(0x03<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_1_1MS	(0x04<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_2_116MS	(0x05<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_4_156MS	(0x06<<3)
#define INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_8_244MS	(0x07<<3)
#define INA226_CONFIGURATION_MODE_POWER_DOWN			(0x00<<0)
#define INA226_CONFIGURATION_MODE_TRIGGERED_SHUNT		(0x01<<0)
#define INA226_CONFIGURATION_MODE_TRIGGERED_BUS			(0x02<<0)
#define INA226_CONFIGURATION_MODE_TRIGGERED_SHUNT_BUS	(0x03<<0)
#define INA226_CONFIGURATION_MODE_CONTINOUS_SHUNT		(0x05<<0)
#define INA226_CONFIGURATION_MODE_CONTINOUS_BUS			(0x06<<0)
#define INA226_CONFIGURATION_MODE_CONTINOUS_SHUNT_BUS	(0x07<<0)

#define INA226_MASK_ENABLE_SHUNT_OVERVOLTAGE			(0x01<<15)
#define INA226_MASK_ENABLE_SHUNT_UNDERVOLTAGE			(0x01<<14)
#define INA226_MASK_ENABLE_BUS_OVERVOLTAGE				(0x01<<13)
#define INA226_MASK_ENABLE_BUS_UNDERVOLTAGE				(0x01<<12)
#define INA226_MASK_ENABLE_POWER_OVERLIMIT				(0x01<<11)
#define INA226_MASK_ENABLE_CONVERSION_READY				(0x01<<10)
#define INA226_MASK_ENABLE_ALERT_FUNCTION_FLAG			(0x01<<4)
#define INA226_MASK_ENABLE_CONVERSION_READY_FLAG		(0x01<<3)
#define INA226_MASK_ENABLE_MATH_OVERFLOW_FLAG			(0x01<<2)
#define INA226_MASK_ENABLE_ALERT_POLARITY_ACTIVE_HIGH	(0x01<<1)
#define INA226_MASK_ENABLE_ALERT_POLARITY_ACTIVE_LOW	(0x00<<1)
#define INA226_MASK_ENABLE_ALERT_LATCH_ENABLE			(0x01<<0)
#define INA226_MASK_ENABLE_ALERT_LATCH_TRANSPARENT		(0x00<<0)

class INA226 : TaskClassS<512> {
	IntervalLogger<comm::Power> telemetry;

	static constexpr float Rshunt = 0.002f;
	static constexpr float max_current = 30.f;

	static constexpr float shunt_voltage_LSB = 0.0000025f;
	static constexpr float bus_voltage_LSB = 0.00125f;
	static constexpr float current_LSB = max_current/32768.f;
	static constexpr float power_LSB = 25.f*current_LSB;

	static constexpr uint16_t calib = 0.00512f/(current_LSB*Rshunt);

	uint8_t buffer[8];

	void init();

public:
	INA226();

	bool readData();

	float getShuntVoltage() const;
	float getBusVoltage() const;
	float getPower() const;
	float getCurrent() const;

	void task();
};

INA226 power_monitor;

INA226::INA226() : TaskClassS{"INA226 reader", TaskPrio_Low},
		telemetry{"INA226 telemetry", Transfer::ID::SENSOR_POWER_MONITOR} {

	static_assert(max_current*Rshunt<0.081f);
}

void INA226::init() {

	SensorBus::getInstance().write(INA226_ADDR, INA226_REG_CONFIGURATION, static_cast<uint16_t>(
		INA226_CONFIGURATION_RESET
	));

	vTaskDelay(100);

	SensorBus::getInstance().write(INA226_ADDR, INA226_REG_CONFIGURATION, static_cast<uint16_t>(
		INA226_CONFIGURATION_AVERAGE_1 |
		INA226_CONFIGURATION_BUS_VOLTAGE_CONV_1_1MS |
		INA226_CONFIGURATION_SHUNT_VOLTAGE_CONV_1_1MS |
		INA226_CONFIGURATION_MODE_CONTINOUS_SHUNT_BUS
	));

	SensorBus::getInstance().write(INA226_ADDR, INA226_REG_CALIBRATION, calib);

	Logger::getInstance().log(Logger::INFO, "pwr: initialization complete");
}

bool INA226::readData() {
	int status = 0;

	memset(buffer, 0, 8);

	if(!status) status = SensorBus::getInstance().read(INA226_ADDR, INA226_REG_SHUNT_VOLTAGE,	&buffer[0], 2);
	if(!status) status = SensorBus::getInstance().read(INA226_ADDR, INA226_REG_BUS_VOLTAGE,		&buffer[2], 2);
	if(!status) status = SensorBus::getInstance().read(INA226_ADDR, INA226_REG_POWER,			&buffer[4], 2);
	if(!status) status = SensorBus::getInstance().read(INA226_ADDR, INA226_REG_CURRENT,			&buffer[6], 2);

	return !status;
}

float INA226::getShuntVoltage() const {
	const int16_t raw = (((uint16_t)buffer[0])<<8) | buffer[1];

	return raw*shunt_voltage_LSB;
}

float INA226::getBusVoltage() const {
	const uint16_t raw = (((uint16_t)buffer[2])<<8) | buffer[3];

	return raw*bus_voltage_LSB;
}

float INA226::getPower() const {
	const uint16_t raw = (((uint16_t)buffer[4])<<8) | buffer[5];

	return raw*power_LSB;
}

float INA226::getCurrent() const {
	const int16_t raw = (((uint16_t)buffer[6])<<8) | buffer[7];

	return raw*current_LSB;
}

void INA226::task() {

	init();

	TickType_t time = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&time, 50);

		if(!readData()) {
			continue;
		}

		comm::Power data;
		data.shunt = getShuntVoltage();
		data.bus = getBusVoltage();
		data.current = getCurrent();
		data.power = getPower();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::VOLTAGE, data.bus, 0);
		Transport::getInstance().sensor_queue.add(Transport::Sensors::CURRENT, data.current, 0);

		telemetry.feed(data);
	}
}

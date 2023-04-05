#include "TaskCPP.h"

#include "logger.h"
#include "transport.h"

class VL53L0X : TaskClassS<1024> {
public:
	VL53L0X();

	void init();

	bool readData();
	float getDistance() const;

	void task();
};

VL53L0X laser;

VL53L0X::VL53L0X() : TaskClassS{"VL53L0X reader", TaskPrio_Low} {

}

void VL53L0X::init() {

}

bool VL53L0X::readData() {
	return false;
}

float VL53L0X::getDistance() const {
	return 0.f;
}

void VL53L0X::task() {

	init();

	while(1) {

		if(!readData()) {
			continue;
		}

		const float dist = getDistance();

		Transport::getInstance().sensor_queue.add(Transport::Sensors::LASER, dist, 0);
	}
}
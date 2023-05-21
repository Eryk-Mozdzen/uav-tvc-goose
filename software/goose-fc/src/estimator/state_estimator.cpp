#include "TimerCPP.h"
#include "TaskCPP.h"
#include "MutexCPP.h"

#include "logger.h"
#include "transport.h"
#include "attitude_estimator.h"
#include "interval_logger.h"

class StateEstimator : TaskClassS<2048> {
	AttitudeEstimator attitude_estimator;

	IntervalLogger<Quaternion> telemetry_attitude;

public:
	StateEstimator();

	void task();
};

StateEstimator estimator;

StateEstimator::StateEstimator() : TaskClassS{"State Estimator", TaskPrio_Mid},
		telemetry_attitude{"attitude telemetry", Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE} {

}

void StateEstimator::task() {

	TickType_t time = xTaskGetTickCount();

	Transport::Sensors type;

	while(1) {
		xTaskDelayUntil(&time, 10);

		while(Transport::getInstance().sensor_queue.pop(type, 0)) {

			switch(type) {
				case Transport::Sensors::ACCELEROMETER: {
					Vector acc;
					Transport::getInstance().sensor_queue.getValue(acc);
					attitude_estimator.feedAcceleration(acc);
				} break;
				case Transport::Sensors::GYROSCOPE: {
					Vector gyr;
					Transport::getInstance().sensor_queue.getValue(gyr);
					attitude_estimator.feedGyration(gyr);
				} break;
				case Transport::Sensors::MAGNETOMETER: {
					Vector mag;
					Transport::getInstance().sensor_queue.getValue(mag);
					attitude_estimator.feedMagneticField(mag);
				} break;
				case Transport::Sensors::BAROMETER: {

				} break;
				case Transport::Sensors::LASER: {

				} break;
				case Transport::Sensors::VOLTAGE: {

				} break;
				case Transport::Sensors::CURRENT: {

				} break;
			}
		}

		const Quaternion q = attitude_estimator.getAttitude();

		telemetry_attitude.feed(q);
	}
}

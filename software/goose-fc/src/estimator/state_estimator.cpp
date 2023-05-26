#include "TimerCPP.h"
#include "TaskCPP.h"
#include "MutexCPP.h"

#include "logger.h"
#include "transport.h"
#include "attitude_estimator.h"
#include "altitude_estimator.h"
#include "battery_estimator.h"
#include "interval_logger.h"

class StateEstimator : TaskClassS<2048> {
	AttitudeEstimator attitude_estimator;
	AltitudeEstimator altitude_estimator;
	BatteryEstimator battery_estimator;

	IntervalLogger<Quaternion> telemetry_attitude;
	IntervalLogger<float> telemetry_altitude;
	IntervalLogger<float> telemetry_battery;
	IntervalLogger<Vector> telemetry_linear_acceleration;

public:
	StateEstimator();

	void task();
};

StateEstimator estimator;

StateEstimator::StateEstimator() : TaskClassS{"State Estimator", TaskPrio_Mid},
		telemetry_attitude{"attitude telemetry", Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE},
		telemetry_altitude{"altitude telemetry", Transfer::ID::TELEMETRY_ESTIMATION_ALTITUDE},
		telemetry_battery{"SOC telemetry", Transfer::ID::TELEMETRY_ESTIMATION_BATTERY_LEVEL},
		telemetry_linear_acceleration{"linear acceleration telemetry", Transfer::ID::TELEMETRY_ESTIMATION_LINEAR_ACCELERATION} {

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
					altitude_estimator.feedAcceleration(acc);
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
					float press;
					Transport::getInstance().sensor_queue.getValue(press);
					altitude_estimator.feedPressure(press);
				} break;
				case Transport::Sensors::LASER: {
					float dist;
					Transport::getInstance().sensor_queue.getValue(dist);
					altitude_estimator.feedDistance(dist);
				} break;
				case Transport::Sensors::VOLTAGE: {
					float voltage;
					Transport::getInstance().sensor_queue.getValue(voltage);
					battery_estimator.feedVoltage(voltage);
				} break;
				case Transport::Sensors::CURRENT: {
					float current;
					Transport::getInstance().sensor_queue.getValue(current);
					battery_estimator.feedCurrent(current);
				} break;
			}
		}

		const Quaternion attitude = attitude_estimator.getAttitude();

		altitude_estimator.feedAttitude(attitude);

		const Vector lin_acc = altitude_estimator.getLinearAcceleration();
		const float altitude = altitude_estimator.getAltitude();
		const float battery = battery_estimator.getStateOfCharge()*100;

		telemetry_attitude.feed(attitude);
		telemetry_altitude.feed(altitude);
		telemetry_battery.feed(battery);
		telemetry_linear_acceleration.feed(lin_acc);
	}
}

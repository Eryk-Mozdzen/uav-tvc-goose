#include "TimerCPP.h"
#include "TaskCPP.h"
#include "MutexCPP.h"

#include "logger.h"
#include "transport.h"
#include "attitude_estimator.h"
#include "position_estimator.h"
#include "battery_estimator.h"
#include "interval_logger.h"

class StateEstimator : TaskClassS<2048> {
	AttitudeEstimator attitude_estimator;
	PositionEstimator position_estimator;
	BatteryEstimator battery_estimator;

	IntervalLogger<comm::Estimator> telemetry_estimator;

public:
	StateEstimator();

	void task();
};

StateEstimator estimator;

StateEstimator::StateEstimator() : TaskClassS{"State Estimator", TaskPrio_Mid},
		telemetry_estimator{"estimator telemetry", Transfer::ID::TELEMETRY_ESTIMATOR} {

}

void StateEstimator::task() {

	TickType_t time = xTaskGetTickCount();

	while(1) {
		xTaskDelayUntil(&time, 10);

		Transport::Sensors type;
		while(Transport::getInstance().sensor_queue.pop(type, 0)) {

			switch(type) {
				case Transport::Sensors::ACCELEROMETER: {
					Vector acc;
					Transport::getInstance().sensor_queue.getValue(acc);
					attitude_estimator.feedAcceleration(acc);
					position_estimator.feedAcceleration(acc);
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
					position_estimator.feedPressure(press);
				} break;
				case Transport::Sensors::LASER: {
					float dist;
					Transport::getInstance().sensor_queue.getValue(dist);
					position_estimator.feedDistance(dist);
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

		position_estimator.setAttitude(attitude);

		const Matrix<3, 1> rpy = attitude.getRollPitchYaw();
		const Vector omega = attitude_estimator.getRotationRates();
		const Vector position = position_estimator.getPosition();
		const Vector velocity = position_estimator.getVelocity();
		const Vector acceleration = position_estimator.getLinearAcceleration();

		comm::Controller::State process_value;
		memcpy(process_value.rpy, &rpy, 3*sizeof(float));
		memcpy(process_value.w, &omega, 3*sizeof(float));
		process_value.z = position.z;
		process_value.vz = velocity.z;

		Transport::getInstance().state_queue.push(process_value, 2);

		comm::Estimator estimator_data;
		memcpy(estimator_data.attitude, &attitude, 4*sizeof(float));
		memcpy(estimator_data.position, &position, 3*sizeof(float));
		memcpy(estimator_data.velocity, &velocity, 3*sizeof(float));
		memcpy(estimator_data.acceleration, &acceleration, 3*sizeof(float));
		estimator_data.soc = battery_estimator.getStateOfCharge();
		estimator_data.ground_pressure = position_estimator.getGroundPressure();

		telemetry_estimator.feed(estimator_data);
	}
}

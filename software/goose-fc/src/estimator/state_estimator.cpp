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

		const Matrix<3, 1> rpy = attitude.getRollPitchYaw();
		const Vector omega = attitude_estimator.getRotationRates();
		const Vector lin_acc = altitude_estimator.getLinearAcceleration();

		comm::Controller::State process_value;
		process_value.rpy[0] = rpy(0, 0);
		process_value.rpy[1] = rpy(0, 1);
		process_value.rpy[2] = rpy(0, 2);
		process_value.w[0] = omega.x;
		process_value.w[1] = omega.y;
		process_value.w[2] = omega.z;
		process_value.z = altitude_estimator.getAltitude();
		process_value.vz = altitude_estimator.getVerticalVelocity();

		Transport::getInstance().state_queue.push(process_value, 2);

		comm::Estimator estimator_data;
		estimator_data.quat[0] = attitude.i;
		estimator_data.quat[1] = attitude.j;
		estimator_data.quat[2] = attitude.k;
		estimator_data.quat[3] = attitude.w;
		estimator_data.linear[0] = lin_acc.x;
		estimator_data.linear[1] = lin_acc.y;
		estimator_data.linear[2] = lin_acc.z;
		estimator_data.z = altitude_estimator.getAltitude();
		estimator_data.vz = altitude_estimator.getVerticalVelocity();
		estimator_data.altitude_src = altitude_estimator.getSource();
		estimator_data.battery_soc = battery_estimator.getStateOfCharge();

		telemetry_estimator.feed(estimator_data);
	}
}

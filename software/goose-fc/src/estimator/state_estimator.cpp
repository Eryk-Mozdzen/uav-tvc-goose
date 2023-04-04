#include "TimerCPP.h"
#include "TaskCPP.h"
#include "MutexCPP.h"

#include "logger.h"
#include "transport.h"

#include "extended_kalman_filter.h"
#include "quaternion.h"
#include "matrix.h"
#include "vector.h"

class StateEstimator : TaskClassS<2048> {
	static constexpr float dt = 0.005;

	ExtendedKalmanFilter<7, 3, 6> ekf;

	Mutex lock;
	Quaternion body_to_world;
	TimerMember<StateEstimator> azimuth_setter;

	Vector acceleration;
	Vector magnetic_field;
	bool acc_ready;
	bool mag_ready;

	Quaternion getAttitudeNED() const;
	Vector removeMagneticDeclination(Vector mag) const;

	void zeroAzimuth();

public:
	StateEstimator();

	Quaternion getAttitude();

	void task();
};

StateEstimator estimator;

StateEstimator::StateEstimator() : TaskClassS{"State Estimator", TaskPrio_Mid}, ekf{

		[](const Matrix<7, 1> state, const Matrix<3, 1> gyr) {
			(void)gyr;

			const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

			const Matrix<7, 7> A = {
				1, 0, 0, 0,  0.5f*dt*q.i,  0.5f*dt*q.j,  0.5f*dt*q.k,
				0, 1, 0, 0, -0.5f*dt*q.w,  0.5f*dt*q.k, -0.5f*dt*q.j,
				0, 0, 1, 0, -0.5f*dt*q.k, -0.5f*dt*q.w,  0.5f*dt*q.i,
				0, 0, 0, 1,  0.5f*dt*q.j, -0.5f*dt*q.i, -0.5f*dt*q.w,

				0, 0, 0, 0,  1,			   0, 		     0,
				0, 0, 0, 0,  0,			   1,		     0,
				0, 0, 0, 0,  0,			   0,   		 1
			};

			const Matrix<7, 3> B = {
				-q.i, -q.j, -q.k,
				q.w, -q.k,  q.j,
				q.k,  q.w, -q.i,
				-q.j,  q.i,  q.w,
				0,    0,    0,
				0,    0,    0,
				0,    0,    0
			};

			return A*state + 0.5f*dt*B*gyr;
		},

		[](const Matrix<7, 1> state) {
			const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

			const Matrix<6, 7> C = {
				 2.f*q.j, -2.f*q.k,  2.f*q.w, -2.f*q.i, 0, 0, 0,
				-2.f*q.i, -2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0,
				-2.f*q.w,  2.f*q.i,  2.f*q.j, -2.f*q.k, 0, 0, 0,

				-2.f*q.k, -2.f*q.j, -2.f*q.i, -2.f*q.w, 0, 0, 0,
				-2.f*q.w,  2.f*q.i, -2.f*q.j,  2.f*q.k, 0, 0, 0,
				 2.f*q.i,  2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0
			};

			return C*state;
		},

		[](const Matrix<7, 1> state, const Matrix<3, 1> gyr) {
			(void)gyr;

			const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

			const Matrix<7, 7> A = {
				1, 0, 0, 0,  0.5f*dt*q.i,  0.5f*dt*q.j,  0.5f*dt*q.k,
				0, 1, 0, 0, -0.5f*dt*q.w,  0.5f*dt*q.k, -0.5f*dt*q.j,
				0, 0, 1, 0, -0.5f*dt*q.k, -0.5f*dt*q.w,  0.5f*dt*q.i,
				0, 0, 0, 1,  0.5f*dt*q.j, -0.5f*dt*q.i, -0.5f*dt*q.w,

				0, 0, 0, 0,  1,			   0, 		     0,
				0, 0, 0, 0,  0,			   1,		     0,
				0, 0, 0, 0,  0,			   0,   		 1
			};

			return A;
		},

		[](const Matrix<7, 1> state) {
			const Quaternion q(state(0, 0), state(1, 0), state(2, 0), state(3, 0));

			const Matrix<6, 7> C = {
				 2.f*q.j, -2.f*q.k,  2.f*q.w, -2.f*q.i, 0, 0, 0,
				-2.f*q.i, -2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0,
				-2.f*q.w,  2.f*q.i,  2.f*q.j, -2.f*q.k, 0, 0, 0,

				-2.f*q.k, -2.f*q.j, -2.f*q.i, -2.f*q.w, 0, 0, 0,
				-2.f*q.w,  2.f*q.i, -2.f*q.j,  2.f*q.k, 0, 0, 0,
				 2.f*q.i,  2.f*q.w, -2.f*q.k, -2.f*q.j, 0, 0, 0
			};

			return C;
		},

		Matrix<7, 7>::identity()*0.0001f,
		Matrix<6, 6>::identity()*1.f,
		{0, -1, 0, 0, 0, 0, 0}},

		lock{"body to world quat lock"},
		azimuth_setter{"azimuth setter", this, &StateEstimator::zeroAzimuth, 3000, pdFALSE} {

}

Vector StateEstimator::removeMagneticDeclination(Vector b_mag) const {
	const Quaternion q = getAttitudeNED();
	const Matrix<3, 3> rot = q.getRotation();

	Vector w_mag = rot*b_mag;

	w_mag.z = 0.f;
	w_mag.normalize();

	b_mag = rot.transposition()*w_mag;

	return b_mag;
}

Quaternion StateEstimator::getAttitudeNED() const {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

	return q;
}

Quaternion StateEstimator::getAttitude() {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

	if(lock.take(100)) {
		const Quaternion result = body_to_world^q;
		lock.give();
		return result;
	}

	return q;
}

void StateEstimator::task() {

	azimuth_setter.start();

	TickType_t time = xTaskGetTickCount();

	Transport::Sensors type;

	while(1) {
		xTaskDelayUntil(&time, 10);

		while(Transport::getInstance().sensor_queue.pop(type, 0)) {

			switch(type) {
				case Transport::Sensors::ACCELEROMETER: {
					Transport::getInstance().sensor_queue.getValue(acceleration);
					acc_ready = true;

				} break;
				case Transport::Sensors::GYROSCOPE: {
					Vector gyration;
					Transport::getInstance().sensor_queue.getValue(gyration);
					ekf.predict(gyration);

				} break;
				case Transport::Sensors::MAGNETOMETER: {
					Transport::getInstance().sensor_queue.getValue(magnetic_field);
					mag_ready = true;

				} break;
				case Transport::Sensors::BAROMETER: {

				} break;
			}

			if(acc_ready) {
				const float len = acceleration.getLength()/9.81f;

				if(len>1.03f || len<0.97f) {
					acc_ready = false;
				}
			}

			if(acc_ready && mag_ready) {
				acc_ready = false;
				mag_ready = false;

				acceleration.normalize();
				magnetic_field.normalize();

				const Vector mag = removeMagneticDeclination(magnetic_field);

				ekf.update({acceleration.x, acceleration.y, acceleration.z, mag.x, mag.y, mag.z});
			}

		}

		const Quaternion q = getAttitude();

		Logger::getInstance().send(Transfer::ID::TELEMETRY_ESTIMATION_ATTITUDE, q);
	}
}

void StateEstimator::zeroAzimuth() {
	const Quaternion q = getAttitudeNED();

	const float azimuth = std::atan2(2.f*(q.w*q.k + q.i*q.j), q.w*q.w + q.i*q.i - q.j*q.j - q.k*q.k);

	if(lock.take(100)) {
		body_to_world = Quaternion(-azimuth, Vector::Z);

		lock.give();

		constexpr float pi = 3.14159265359f;

		Logger::getInstance().log(Logger::INFO, "est: set azimuth to %.0f deg", (double)(azimuth*180.f/pi));
	}
}

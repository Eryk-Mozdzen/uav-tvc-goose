#include "state_estimator.h"

#include <cmath>

#include "communication.h"
#include "sensors.h"
#include "queue_element.h"
#include "matrix.h"
#include "vector.h"

StateEstimator::StateEstimator() : ekf{

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
		{1, 0, 0, 0, 0, 0, 0}} {

}

void StateEstimator::handleReading(queue_element_t reading) {

	switch(reading.type) {
		case Sensors::ACCELEROMETER: {
			memcpy(&acceleration, reading.data, sizeof(Vector));
			acc_ready = true;

		} break;
		case Sensors::GYROSCOPE: {
			Vector gyration;
			memcpy(&gyration, reading.data, sizeof(Vector));
			ekf.predict(gyration);

		} break;
		case Sensors::MAGNETOMETER: {
			memcpy(&magnetic_field, reading.data, sizeof(Vector));
			mag_ready = true;

		} break;
		case Sensors::BAROMETER: {
			
		} break;
		default: {
			COM.log(Communication::WARNING, "est: unknown sensor reading\n\r");
		} break;
	}

	if(acc_ready) {
		const float len = acceleration.getLength()/9.81f;

		if(len>1.03f || len<0.97f) {
			COM.log(Communication::INFO, "est: linear acceleration detected, total length: %f\n\r", (double)len);
			acc_ready = false;
		}
	}

	if(acc_ready && mag_ready) {
		acc_ready = false;
		mag_ready = false;

		acceleration.normalize();
		magnetic_field.normalize();

		const Vector mag = remove_earth_declination(magnetic_field);

		ekf.update({acceleration.x, acceleration.y, acceleration.z, mag.x, mag.y, mag.z});
	}
}

Vector StateEstimator::remove_earth_declination(Vector b_mag) const {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();
	const Matrix<3, 3> rot = q.getRotation();

	Vector w_mag = rot*b_mag;

	w_mag.z = 0.f;
	w_mag.normalize();

	b_mag = rot.transposition()*w_mag;

	return b_mag;
}

Quaternion StateEstimator::getAttitude() const {
	const Matrix<7, 1> state = ekf.getState();
	const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

	return q;
}

void estimatorTaskFcn(void *param) {
	QueueHandle_t sensor_queue = static_cast<QueueHandle_t>(param);

	StateEstimator estimator;

	TickType_t time = xTaskGetTickCount();

	while(1) {
		xTaskDelayUntil(&time, 5);

		queue_element_t reading;

		while(xQueueReceive(sensor_queue, &reading, 0)) {
			estimator.handleReading(reading);
		}

		const Quaternion q = estimator.getAttitude();

		COM.log(Communication::DEBUG, "quat: %+10.5f %+10.5f %+10.5f %+10.5f\n\r", (double)(q.w), (double)(q.i), (double)(q.j), (double)(q.k));
	}
}

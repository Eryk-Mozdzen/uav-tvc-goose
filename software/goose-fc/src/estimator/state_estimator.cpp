#include "state_estimator.h"

#include <cmath>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "logger.h"
#include "queue_element.h"
#include "matrix.h"
#include "kalman_filter.h"
#include "extended_kalman_filter.h"
#include "quaternion.h"

QueueHandle_t state_queue;

static float3_t acceleration;
static float3_t gyration;
static float3_t magnetic_field;
static float pressure;

void handle_readings() {
	queue_element_t reading;

	while(xQueueReceive(sensor_queue, &reading, 0)) {

		switch(reading.type) {
			case SENSOR_ACCELEROMETER: {
				memcpy(&acceleration, reading.data, sizeof(float3_t));

			} break;
			case SENSOR_GYROSCOPE: {
				memcpy(&gyration, reading.data, sizeof(float3_t));

			} break;
			case SENSOR_MAGNETOMETER: {
				memcpy(&magnetic_field, reading.data, sizeof(float3_t));

			} break;
			case SENSOR_BAROMETER: {
				memcpy(&pressure, reading.data, sizeof(float));

			} break;
			default: {
				LOG(LOG_WARNING, "est: unknown sensor reading\n\r");
			} break;
		}

		vPortFree(reading.data);
	}
}

float3_t normalize(float3_t vec) {
	const float len = sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	
	if(len<0.01f)
		return vec;

	vec.x /=len;
	vec.y /=len;
	vec.z /=len;
	
	return vec;
}

void estimator(void *param) {
	(void)param;

	TickType_t time = xTaskGetTickCount();

	constexpr float pi = 3.1415f;
	constexpr float rad_to_deg = 180.f/pi;

	constexpr float dt = 0.01;

	const auto f = [](const Matrix<7, 1> state, const Matrix<3, 1> gyr) {
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
	};

	const auto h = [](const Matrix<7, 1> state) {
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
	};

	const auto f_tangent = [](const Matrix<7, 1> state, const Matrix<3, 1> gyr) {
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
	};

	const auto h_tangent = [](const Matrix<7, 1> state) {
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
	};

	constexpr Matrix<7, 7> Q = Matrix<7, 7>::identity()*0.00001f;
	constexpr Matrix<6, 6> R = Matrix<6, 6>::identity()*1.f;
	constexpr Matrix<7, 1> x_init = {1, 0, 0, 0, 0, 0, 0};

	ExtendedKalmanFilter<7, 3, 6> ekf(f, h, f_tangent, h_tangent, Q, R, x_init);

	while(1) {
		xTaskDelayUntil(&time, 10);

		handle_readings();

		ekf.predict({gyration.x, gyration.y, gyration.z});

		const float3_t acc = normalize(acceleration);
		const float3_t mag = normalize(magnetic_field);

		ekf.update({acc.x, acc.y, acc.z, mag.x, mag.y, mag.z});

		const Matrix<7, 1> state = ekf.getState();
		const Quaternion q = Quaternion(state(0, 0), state(1, 0), state(2, 0), state(3, 0)).getNormalized();

		LOG(LOG_DEBUG, "quat: %+10.5f %+10.5f %+10.5f %+10.5f\n\r", (double)(q.w), (double)(q.i), (double)(q.j), (double)(q.k));
	}
}

void StateEstimator_Init() {
	state_queue = xQueueCreate(16, sizeof(state_t));

	xTaskCreate(estimator, "state estimator", 1024, NULL, 4, NULL);
}

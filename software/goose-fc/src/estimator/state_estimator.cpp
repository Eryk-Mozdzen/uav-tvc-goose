#include "state_estimator.h"

#include <cmath>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "logger.h"
#include "queue_element.h"
#include "matrix.h"
#include "kalman_filter.h"

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

float3_t normalize(const float3_t vec) {
	const float len = sqrtf(acceleration.x*acceleration.x + acceleration.y*acceleration.y + acceleration.z*acceleration.z);
	
	if(len<0.01f)
		return vec;

	float3_t norm = vec;
	norm.x /=len;
	norm.y /=len;
	norm.z /=len;
	
	return norm;
}

void estimator(void *param) {
	(void)param;

	TickType_t time = xTaskGetTickCount();

	const float pi = 3.1415f;
	const float rad_to_deg = 180.f/pi;

	const float dt = 0.01;

	const Matrix<6, 6> F = {
		1, 0, 0, dt, 0,  0,
		0, 1, 0, 0,  dt, 0,
		0, 0, 1, 0,  0,  dt, 
		0, 0, 0, 1,  0,  0, 
		0, 0, 0, 0,  1,  0,
		0, 0, 0, 0,  0,  1
	};

	const Matrix<6, 3> B = {
		dt, 0,  0,
		0,  dt, 0,
		0,  0,  dt,
		0,  0,  0,
		0,  0,  0,
		0,  0,  0
	};

	const Matrix<3, 6> H = {
		1, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0
	};

	const Matrix<6, 6> Q = Matrix<6, 6>::identity()*0.005f;
	const Matrix<3, 3> R = Matrix<3, 3>::identity()*1.f;

	KalmanFilter kalman(F, B, H, Q, R, {0, 0, 0});

	while(1) {
		xTaskDelayUntil(&time, 10);

		handle_readings();

		float tmp = acceleration.x;
		acceleration.x = -acceleration.y;
		acceleration.y = -tmp;

		acceleration = normalize(acceleration);
		magnetic_field = normalize(magnetic_field);

		const float roll  = atan2f(acceleration.y, acceleration.z);
		const float pitch = atan2f(-acceleration.x, sqrtf(acceleration.y*acceleration.y + acceleration.z*acceleration.z));

		const float cos_roll = cosf(pitch);
		const float sin_roll = sinf(pitch);
		const float cos_pitch = cosf(roll);
		const float sin_pitch = sinf(roll);

		const float yaw = atan2f(
			-magnetic_field.y*cos_roll + magnetic_field.z*sin_roll, 
			magnetic_field.x*cos_pitch + magnetic_field.y*sin_pitch*sin_roll + magnetic_field.z*sin_pitch*cos_roll
		);

		kalman.predict({gyration.y, gyration.x, gyration.z});

		kalman.update({roll, pitch, yaw});

		const Matrix<6, 1> attitude = kalman.getState();

		//LOG(LOG_DEBUG, "acc: %+10.2f %+10.2f %+10.2f m/s2\n\r", (double)acceleration.x, (double)acceleration.y, (double)acceleration.z);
		//LOG(LOG_DEBUG, "gyr: %+10.2f %+10.2f %+10.2f rad/s\n\r", (double)gyration.x, (double)gyration.y, (double)gyration.z);
		//LOG(LOG_DEBUG, "mag: %+10.2f %+10.2f %+10.2f\n\r", (double)magnetic_field.x, (double)magnetic_field.y, (double)magnetic_field.z);

		//LOG(LOG_DEBUG, "rpy: %7.2f %7.2f %7.2f\n\r", (double)(attitude.x*rad_to_deg), (double)(attitude.y*rad_to_deg), (double)(attitude.z*rad_to_deg));

		LOG(LOG_DEBUG, "rpy: %7.2f %7.2f %7.2f\n\r", (double)(attitude(0, 0)*rad_to_deg), (double)(attitude(1, 0)*rad_to_deg), (double)(attitude(2, 0)*rad_to_deg));
	}
}

void StateEstimator_Init() {
	state_queue = xQueueCreate(16, sizeof(state_t));

	xTaskCreate(estimator, "state estimator", 1024, NULL, 4, NULL);
}

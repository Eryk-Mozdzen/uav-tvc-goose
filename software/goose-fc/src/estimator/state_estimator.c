#include "state_estimator.h"

#include <math.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "logger.h"
#include "queue_element.h"

QueueHandle_t state_queue;

static float3_t acceleration;
static float3_t gyration;
static float3_t magnetic_field;
static float pressure;

static void handle_readings() {
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

static float3_t normalize(const float3_t vec) {
	const float len = sqrtf(acceleration.x*acceleration.x + acceleration.y*acceleration.y + acceleration.z*acceleration.z);
	
	if(len<0.01f)
		return (float3_t){0};
	
	return (float3_t){
		vec.x/len,
		vec.y/len,
		vec.z/len
	};
}

static void estimator(void *param) {
	(void)param;

	TickType_t time = xTaskGetTickCount();

	const float pi = 3.1415f;

	while(1) {
		xTaskDelayUntil(&time, 10);

		handle_readings();

		float tmp = acceleration.x;
		acceleration.x = -acceleration.y;
		acceleration.y = -tmp;

		acceleration = normalize(acceleration);
		magnetic_field = normalize(magnetic_field);

		float3_t attitude = {
			.x = atan2f(acceleration.y, acceleration.z),
			.y = atan2f(-acceleration.x, sqrtf(acceleration.y*acceleration.y + acceleration.z*acceleration.z)),
			.z = atan2f(magnetic_field.y, magnetic_field.x)
		};

		const float cos_roll = cosf(attitude.y);
		const float sin_roll = sinf(attitude.y);
		const float cos_pitch = cosf(attitude.x);
		const float sin_pitch = sinf(attitude.x);
		attitude.z = atan2f(
			-magnetic_field.y*cos_roll + magnetic_field.z*sin_roll, 
			magnetic_field.x*cos_pitch + magnetic_field.y*sin_pitch*sin_roll + magnetic_field.z*sin_pitch*cos_roll
		);

		const float rad_to_deg = 180.f/pi;

		//LOG(LOG_DEBUG, "acc: %+10.2f %+10.2f %+10.2f m/s2\n\r", (double)acceleration.x, (double)acceleration.y, (double)acceleration.z);
		//LOG(LOG_DEBUG, "gyr: %+10.2f %+10.2f %+10.2f rad/s\n\r", (double)gyration.x, (double)gyration.y, (double)gyration.z);
		//LOG(LOG_DEBUG, "mag: %+10.2f %+10.2f %+10.2f\n\r", (double)magnetic_field.x, (double)magnetic_field.y, (double)magnetic_field.z);

		LOG(LOG_DEBUG, "rpy: %7.2f %7.2f %7.2f\n\r", (double)(attitude.x*rad_to_deg), (double)(attitude.y*rad_to_deg), (double)(attitude.z*rad_to_deg));
	}
}

void StateEstimator_Init() {
	state_queue = xQueueCreate(16, sizeof(state_t));

	xTaskCreate(estimator, "state estimator", 1024, NULL, 4, NULL);
}

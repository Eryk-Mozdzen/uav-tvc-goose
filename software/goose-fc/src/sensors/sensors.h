#ifndef SENSORS_H
#define SENSORS_H

#include "FreeRTOS.h"
#include "queue.h"

typedef struct {
	float x, y, z;
} float3_t;

typedef enum {
	SENSOR_ACCELEROMETER,
	SENSOR_GYROSCOPE,
	SENSOR_MAGNETOMETER,
	SENSOR_BAROMETER
} sensor_t;

extern QueueHandle_t sensor_queue;

void Sensors_Init();

#endif

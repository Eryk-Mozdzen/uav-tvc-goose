#ifndef SENSORS_H
#define SENSORS_H

#include "FreeRTOS.h"
#include "queue.h"

typedef struct {
	float x, y, z;
} float3_t;

extern QueueHandle_t acc_queue;
extern QueueHandle_t gyr_queue;
extern QueueHandle_t mag_queue;
extern QueueHandle_t bar_queue;

void Sensors_Init();

#endif

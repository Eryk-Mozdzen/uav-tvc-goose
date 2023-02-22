#ifndef STATE_ESTIMATOR_H
#define STATE_ESTIMATOR_H

#include "sensors.h"

typedef struct {
	float3_t acceleration;
	float3_t gyration;
	float3_t magnetic_field;
	float pressure;
	float voltage;
	float current;

	float3_t attitude;
	float3_t linear_acceleration;
	float altitude;
} telemetry_t;

typedef struct {
	float3_t attitude;
	float altitude;
} state_t;

extern QueueHandle_t state_queue;

#ifdef __cplusplus
extern "C"
#endif
void StateEstimator_Init();

#endif

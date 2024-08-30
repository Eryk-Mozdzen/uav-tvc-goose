// auto-generated file
// 2024-08-31 00:37:17

#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include "ekf.h"

extern ekf_t ekf;
extern ekf_system_model_t system_model;
extern ekf_measurement_model_t magnetometer_model;
extern ekf_measurement_model_t gravity_model;
extern ekf_measurement_model_t rangefinder_model;
extern ekf_measurement_model_t barometer_model;
extern ekf_measurement_model_t gps_model;
extern ekf_measurement_model_t flow_model;

EKF_PREDICT_DEF(12, 6)
EKF_CORRECT_DEF(12, 1)
EKF_CORRECT_DEF(12, 2)
EKF_CORRECT_DEF(12, 3)

#endif

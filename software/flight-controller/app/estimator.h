// auto-generated file
// 2024-09-01 11:04:31

#ifndef ESTIMATOR_H
#define ESTIMATOR_H

#include "ekf.h"

extern ekf_t ekf;
extern ekf_system_model_t system_model;
extern ekf_measurement_model_t magnetometer_model;
extern ekf_measurement_model_t rangefinder_model;
extern ekf_measurement_model_t barometer_model;
extern ekf_measurement_model_t gps_model;
extern ekf_measurement_model_t flow_model;

EKF_PREDICT_DEF(15, 6)
EKF_CORRECT_DEF(15, 1)
EKF_CORRECT_DEF(15, 2)
EKF_CORRECT_DEF(15, 3)

#endif

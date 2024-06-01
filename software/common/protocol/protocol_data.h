#ifndef PROTOCOL_DATA_H
#define PROTOCOL_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    PROTOCOL_ID_LOG,
    PROTOCOL_ID_READINGS,
    PROTOCOL_ID_PASSTHROUGH_GPS,
    PROTOCOL_ID_ESTIMATION,
    PROTOCOL_ID_CALIBRATION,
    PROTOCOL_ID_CONTROL
} protocol_id_t;

typedef struct {
    struct {
        float magnetometer[3];
        float accelerometer[3];
        float gyroscope[3];
    } raw;
    struct {
        float magnetometer[3];
        float accelerometer[3];
        float gyroscope[3];
    } calibrated;
    float rangefinder;
    float barometer;
    float gps[2];
    union {
        struct {
            uint8_t magnetometer : 1;
            uint8_t accelerometer : 1;
            uint8_t gyroscope : 1;
            uint8_t rangefinder : 1;
            uint8_t barometer : 1;
            uint8_t gps : 1;
            uint8_t unused : 2;
        } valid;
        uint8_t valid_all;
    };
} protocol_readings_t;

typedef struct {
    float orientation[4];
    float position[3];
    float velocity[3];
    float theta_d;
    float pressure_0;
} protocol_estimation_t;

typedef struct {
    float magnetometer[12];
    float accelerometer[12];
    float gyroscope[3];
    uint16_t servos[12];
} protocol_calibration_t;

typedef struct {
    uint16_t servos[4];
    uint16_t motor;
} protocol_control_t;

#ifdef __cplusplus
}
#endif

#endif

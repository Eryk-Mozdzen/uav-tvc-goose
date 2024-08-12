#ifndef MSG_H
#define MSG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    MSG_ID_LOG,
    MSG_ID_PASSTHROUGH_GPS,
    MSG_ID_IMU,
	MSG_ID_BAROMETER,
	MSG_ID_POWER,
	MSG_ID_RANGEFINDER,
	MSG_ID_TACHOMETER,
    MSG_ID_FLOW,
    MSG_ID_GPS,
    MSG_ID_ESTIMATION,
    MSG_ID_CONTROLLER,
    MSG_ID_CALIBRATION,
    MSG_ID_CONTROL,
    MSG_ID_COMMAND_START,
    MSG_ID_COMMAND_ABORT,
} msg_id_t;

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
} msg_frame_imu_t;

typedef struct {
    float pressure;
} msg_frame_barometer_t;

typedef struct {
    float voltage;
    float current;
} msg_frame_power_t;

typedef struct {
    float distance;
} msg_frame_rangefinder_t;

typedef struct {
    float velocity;
} msg_frame_tachometer_t;

typedef struct {
    float delta[2];
} msg_frame_flow_t;

typedef struct {
    union {
        struct {
            float latitude;
            float longitude;
        } earth;
        float position[2];
    };
} msg_frame_gps_t;

typedef struct {
    float position[3];
    float velocity[3];
    float orientation[4];
    float angular_velocity[3];
    float theta_d;
    float pressure_0;
} msg_frame_estimation_t;

typedef struct {
    float magnetometer[12];
    float accelerometer[12];
    float gyroscope[3];
    uint16_t servos[9];
} msg_frame_calibration_t;

typedef struct {
    uint16_t motor;
    uint16_t servos[3];
} msg_frame_control_t;

#ifdef __cplusplus
}
#endif

#endif

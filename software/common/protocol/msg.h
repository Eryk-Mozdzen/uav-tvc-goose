#ifndef MSG_H
#define MSG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    MSG_ID_LOG,
    MSG_ID_PASSTHROUGH_GPS,
    MSG_ID_SENSOR,
    MSG_ID_ESTIMATION,
    MSG_ID_CALIBRATION,
    MSG_ID_SETPOINT,
    MSG_ID_CONTROLLER,
    MSG_ID_MANUAL,
    MSG_ID_COMMAND_REFERENCE,
    MSG_ID_COMMAND_START,
    MSG_ID_COMMAND_ABORT,
} msg_id_t;

typedef enum {
    MSG_SM_STATE_ABORT,
    MSG_SM_STATE_READY,
    MSG_SM_STATE_ACTIVE,
    MSG_SM_STATE_MANUAL,
} msg_sm_state_t;

typedef struct {
    struct {
        float raw[3];
        float calib[3];
    } magnetometer;
    struct {
        float raw[3];
        float calib[3];
    } accelerometer;
    struct {
        float raw[3];
        float calib[3];
    } gyroscope;
    struct {
        uint32_t raw;
        float calib;
    } load;
    float barometer;
    float rangefinder;
    float tachometer;
    float power[2];
    float flow[2];
    float gps[2];
    union {
        struct {
            uint32_t magnetometer : 1;
            uint32_t accelerometer : 1;
            uint32_t gyroscope : 1;
            uint32_t load : 1;
            uint32_t barometer : 1;
            uint32_t rangefinder : 1;
            uint32_t tachometer : 1;
            uint32_t power : 1;
            uint32_t flow : 1;
            uint32_t gps : 1;
            uint32_t unused : 22;
        } valid;
        uint32_t valid_all;
    };
} msg_frame_sensor_t;

typedef struct {
    float position[3];
    float velocity[3];
    float orientation[4];
    float angular_velocity[3];
    float theta_d;
    float pressure_0;
    struct {
        float latlon[2];
        uint32_t valid : 1;
        uint32_t unused : 31;
    } position_reference;
} msg_frame_estimation_t;

typedef struct {
    float magnetometer[12];
    float accelerometer[12];
    float gyroscope[3];
    uint32_t servos[9];
    uint32_t load[2];
} msg_frame_calibration_t;

typedef struct {
    float rpy[3];
    float omega[3];
    float pos[3];
    float vel[3];
} msg_frame_setpoint_t;

typedef struct {
    struct {
        float rpy[3];
        float omega[3];
        float pos[3];
        float vel[3];
    } process;
    struct {
        float rpy[3];
        float omega[3];
        float pos[3];
        float vel[3];
    } setpoint;
    struct {
        float throttle;
        float angles[3];
    } controls;
    float core_load;
    uint32_t state;
} msg_frame_controller_t;

typedef struct {
    union {
        uint32_t raw[3];
        float calibrated[3];
    } servos;
    uint32_t motor : 8;
    uint32_t is_raw : 1;
    uint32_t unused : 23;
} msg_frame_manual_t;

#ifdef __cplusplus
}
#endif

#endif

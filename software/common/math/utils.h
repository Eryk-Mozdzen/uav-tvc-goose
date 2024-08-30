#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

#ifndef PI
    #define PI    3.14159265359f
#endif
#define RAD2DEG   57.2957795131f
#define DEG2RAD   0.01745329251f

float utils_length(const float *vec, const size_t dim);
void utils_normalize(const float *src, float *dest, const size_t dim);
void utils_quaternion_to_rpy(const float *quaternion, float *rpy);
void utils_quaternion_to_rot(const float *quaternion, float *rot);
void utils_quaternion_to_rot_trans(const float *quaternion, float *rot);
void utils_gps_to_enu(const float *position, float *cartesian);

#ifdef __cplusplus
}
#endif

#endif

#include <stddef.h>
#include <math.h>

float utils_length(const float *vec, const size_t dim) {
    float sum = 0;

    for(size_t i=0; i<dim; i++) {
        sum +=vec[i]*vec[i];
    }

    return sqrtf(sum);
}

void utils_normalize(const float *src, float *dest, const size_t dim) {
    float sum = 0;

    for(size_t i=0; i<dim; i++) {
        sum +=src[i]*src[i];
    }

    float len = sqrtf(sum);

    if(len<0.001f) {
        len = 0.001f;
    }

    for(size_t i=0; i<dim; i++) {
        dest[i] = src[i] / len;
    }
}

void utils_quaternion_to_rpy(const float *quaternion, float *rpy) {
    const float qx = quaternion[0];
	const float qy = quaternion[1];
	const float qz = quaternion[2];
	const float qw = quaternion[3];

	const float t0 = 2.f * (qw * qx + qy * qz);
	const float t1 = 1.f - 2.f * (qx * qx + qy * qy);
	const float roll = atan2f(t0, t1);

	float t2 = 2.f * (qw * qy - qz * qx);
	t2 = t2 > 1.f ? 1.f : t2;
	t2 = t2 < -1.f ? -1.f : t2;
	const float pitch = asinf(t2);

	const float t3 = 2.f * (qw * qz + qx * qy);
	const float t4 = 1.f - 2.f * (qy * qy + qz * qz);
	const float yaw = atan2f(t3, t4);

    rpy[0] = roll;
    rpy[1] = pitch;
    rpy[2] = yaw;
}

void utils_quaternion_to_rot(const float *quaternion, float *rot) {
    const float qw = quaternion[0];
	const float qx = quaternion[1];
	const float qy = quaternion[2];
	const float qz = quaternion[3];

    const float n = qw*qw + qx*qx + qy*qy + qz*qz;
    const float s = (n>0.001f) ? 2.f/n : 0.f;

    rot[0] = 1.f - s*(qy*qy + qz*qz);
    rot[1] = s*(qx*qy - qw*qz);
    rot[2] = s*(qx*qz + qw*qy);
    rot[3] = s*(qx*qy + qw*qz);
    rot[4] = 1.f - s*(qx*qx + qz*qz);
    rot[5] = s*(qy*qz - qw*qx);
    rot[6] = s*(qx*qz - qw*qy);
    rot[7] = s*(qy*qz + qw*qx);
    rot[8] = 1.f - s*(qx*qx + qy*qy);
}

void utils_quaternion_to_rot_trans(const float *quaternion, float *rot) {
    const float qw = quaternion[0];
	const float qx = quaternion[1];
	const float qy = quaternion[2];
	const float qz = quaternion[3];

    const float n = qw*qw + qx*qx + qy*qy + qz*qz;
    const float s = (n>0.001f) ? 2.f/n : 0.f;

    rot[0] = 1.f - s*(qy*qy + qz*qz);
    rot[1] = s*(qx*qy + qw*qz);
    rot[2] = s*(qx*qz - qw*qy);
    rot[3] = s*(qx*qy - qw*qz);
    rot[4] = 1.f - s*(qx*qx + qz*qz);
    rot[5] = s*(qy*qz + qw*qx);
    rot[6] = s*(qx*qz + qw*qy);
    rot[7] = s*(qy*qz - qw*qx);
    rot[8] = 1.f - s*(qx*qx + qy*qy);
}

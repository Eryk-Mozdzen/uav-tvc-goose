#include "quaternion.h"

Quaternion::Quaternion() : w{1.f}, i{0.f}, j{0.f}, k{0.f} {

}

Quaternion::Quaternion(const float w_p, const float i_p, const float j_p, const float k_p) : w{w_p}, i{i_p}, j{j_p}, k{k_p} {

}

Quaternion::Quaternion(const Matrix<4, 1> &matrix) : w{matrix(0, 0)}, i{matrix(1, 0)}, j{matrix(2, 0)}, k{matrix(3, 0)} {

}

Quaternion::Quaternion(const Matrix<3, 1> &vec) : w{0.f}, i{vec(0, 0)}, j{vec(1, 0)}, k{vec(2, 0)} {

}

Quaternion::Quaternion(const float angle, const Matrix<3, 1> &axis) : w{cosf(0.5f*angle)} {
	const float len = sqrtf(axis(0, 0)*axis(0, 0) + axis(1, 0)*axis(1, 0) + axis(2, 0)*axis(2, 0));
	const float s = sinf(0.5f*angle);

	i = s*axis(0, 0)/len;
	j = s*axis(1, 0)/len;
	k = s*axis(2, 0)/len;
}

Quaternion Quaternion::operator*(const float rhs) const {
	return Quaternion(
		w*rhs,
		i*rhs,
		j*rhs,
		k*rhs
	);
}

Quaternion Quaternion::operator/(const float rhs) const {
	return Quaternion(
		w/rhs,
		i/rhs,
		j/rhs,
		k/rhs
	);
}

Quaternion Quaternion::operator+(const Quaternion &rhs) const {
	return Quaternion(
		w + rhs.w,
		i + rhs.i,
		j + rhs.j,
		k + rhs.k
	);
}

Quaternion Quaternion::operator^(const Quaternion &rhs) const {
	return Quaternion(
		w*rhs.w - i*rhs.i - j*rhs.j - k*rhs.k,
		w*rhs.i + i*rhs.w + j*rhs.k - k*rhs.j,
		w*rhs.j - i*rhs.k + j*rhs.w + k*rhs.i,
		w*rhs.k + i*rhs.j - j*rhs.i + k*rhs.w
	);
}

float Quaternion::operator*(const Quaternion &rhs) const {
	return (
		w * rhs.w +
		i * rhs.i +
		j * rhs.j +
		k * rhs.k
	);
}

float Quaternion::abs() const {
	return sqrtf(w*w + i*i + j*j + k*k);
}

Quaternion Quaternion::conjugation() const {
	return Quaternion(w, -i, -j, -k);
}

Quaternion Quaternion::reciprocal() const {
	const float len = abs();
	return conjugation()/(len*len);
}

Quaternion Quaternion::getNormalized() const {
	return *this/abs();
}

Matrix<3, 3> Quaternion::getRotation() const {
	const float s = 1.f/(w*w + i*i + j*j + k*k);

	return {
		1.f - 2.f*s*(j*j + k*k),	2.f*s*(i*j - k*w), 			2.f*s*(i*k + j*w),
		2.f*s*(i*j + k*w),		 	1.f - 2.f*s*(i*i + k*k),	2.f*s*(j*k - i*w),
		2.f*s*(i*k - j*w),			2.f*s*(j*k + i*w),			1.f - 2.f*s*(i*i + j*j)
	};
}

Matrix<3, 1> Quaternion::getRollPitchYaw() const {
	const float qx = i;
	const float qy = j;
	const float qz = k;
	const float qw = w;

	const float ysqr = qy * qy;
	const float t0 = 2.f * (qw * qx + qy * qz);
	const float t1 = 1.f - 2.f * (qx * qx + ysqr);
	const float roll = atan2f(t0, t1);

	float t2 = 2.f * (qw * qy - qz * qx);
	t2 = t2 > 1.f ? 1.f : t2;
	t2 = t2 < -1.f ? -1.f : t2;
	const float pitch = asinf(t2);

	const float t3 = 2.f * (qw * qz + qx * qy);
	const float t4 = 1.f - 2.f * (ysqr + qz * qz);
	const float yaw = atan2f(t3, t4);

    return {roll, pitch, yaw};
}

void Quaternion::normalize() {
	const float len = abs();

	w /=len;
	i /=len;
	j /=len;
	k /=len;
}

Quaternion operator*(const float lhs, const Quaternion &rhs) {
	return Quaternion(
		rhs.w*lhs,
		rhs.i*lhs,
		rhs.j*lhs,
		rhs.k*lhs
	);
}

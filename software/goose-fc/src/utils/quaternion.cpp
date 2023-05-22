#include "quaternion.h"

Quaternion::Quaternion() : w{1.f}, i{0.f}, j{0.f}, k{0.f} {

}

Quaternion::Quaternion(const float w_p, const float i_p, const float j_p, const float k_p) : w{w_p}, i{i_p}, j{j_p}, k{k_p} {

}

Quaternion::Quaternion(const Matrix<4, 1> &matrix) : w{matrix(0, 0)}, i{matrix(1, 0)}, j{matrix(2, 0)}, k{matrix(3, 0)} {

}

Quaternion::Quaternion(const Matrix<3, 1> &vec) : w{0.f}, i{vec(0, 0)}, j{vec(1, 0)}, k{vec(2, 0)} {

}

Quaternion::Quaternion(const float angle, const Matrix<3, 1> &axis) : w{std::cos(0.5f*angle)} {
	const float len = std::sqrt(axis(0, 0)*axis(0, 0) + axis(1, 0)*axis(1, 0) + axis(2, 0)*axis(2, 0));
	const float s = std::sin(0.5f*angle);

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
	return std::sqrt(w*w + i*i + j*j + k*k);
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
    const float sinr_cosp = 2.f*(w*i + j*k);
    const float cosr_cosp = 1.f - 2.f*(i*i + j*j);
    const float roll = atan2f(sinr_cosp, cosr_cosp);

    const float sinp = 2.f*(w*j - k*i);
	float pitch;
    if(std::abs(sinp)>1.f) {
        pitch = copysignf(3.1415f/2.f, sinp);
    } else {
        pitch = asinf(sinp);
	}

    const float  siny_cosp = 2.f*(w*k + i*j);
    const float  cosy_cosp = 1.f - 2.f*(j*j + k*k);
    const float yaw = atan2f(siny_cosp, cosy_cosp);

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

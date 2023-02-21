#include "quaternion.h"
#include <cmath>

Quaternion::Quaternion() : w{0.f}, i{0.f}, j{0.f}, k{0.f} {

}

Quaternion::Quaternion(const float w_p, const float i_p, const float j_p, const float k_p) : w{w_p}, i{i_p}, j{j_p}, k{k_p} {

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
		w*rhs.w + i*rhs.i + j*rhs.j - k*rhs.k,
		w*rhs.w - i*rhs.i + j*rhs.j + k*rhs.k,
		w*rhs.w + i*rhs.i - j*rhs.j + k*rhs.k
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

Quaternion operator*(const float lhs, const Quaternion &rhs) {
	return rhs*lhs;
}

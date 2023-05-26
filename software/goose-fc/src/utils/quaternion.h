#pragma once

#include "matrix.h"

struct Quaternion {
	float w, i, j, k;

	Quaternion();
	Quaternion(float, float, float, float);
	Quaternion(const Matrix<4, 1> &);
	Quaternion(const Matrix<3, 1> &);
	Quaternion(const float, const Matrix<3, 1> &);

	Quaternion operator*(const float) const;
	Quaternion operator/(const float) const;
	Quaternion operator+(const Quaternion &) const;
	Quaternion operator^(const Quaternion &) const;
	float operator*(const Quaternion &) const;

	float abs() const;
	Quaternion conjugation() const;
	Quaternion reciprocal() const;
	Quaternion getNormalized() const;

	void normalize();

	Matrix<3, 3> getRotation() const;
	Matrix<3, 1> getRollPitchYaw() const;
};

Quaternion operator*(const float, const Quaternion &);

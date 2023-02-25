#pragma once

#include "matrix.h"

struct Vector {
	float x, y, z;

	Vector();
	Vector(float, float, float);
	Vector(const Matrix<3, 1> &);

	operator Matrix<3, 1>() const;

	Vector operator*(const float) const;
	Vector operator/(const float) const;
	Vector operator+(const Vector &) const;
	Vector operator-(const Vector &) const;
	Vector operator^(const Vector &) const;
	float operator*(const Vector &) const;

	void normalize();

	float getLength() const;
	Vector getNormalized() const;
};

Vector operator*(const float, const Vector &);
Vector operator*(const Matrix<3, 3> &, const Vector &);

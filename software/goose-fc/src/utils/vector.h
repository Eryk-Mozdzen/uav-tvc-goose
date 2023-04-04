#pragma once

struct Vector {
	static const Vector X;
	static const Vector Y;
	static const Vector Z;

	float x, y, z;

	Vector();
	Vector(float, float, float);

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

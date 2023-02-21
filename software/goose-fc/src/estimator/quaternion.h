#pragma once

struct Quaternion {
	float w, i, j, k;

	Quaternion();
	Quaternion(float, float, float, float);

	Quaternion operator*(const float) const;
	Quaternion operator/(const float) const;
	Quaternion operator+(const Quaternion &) const;
	Quaternion operator^(const Quaternion &) const;
	float operator*(const Quaternion &) const;

	float abs() const;
};

Quaternion operator*(const float, const Quaternion &);

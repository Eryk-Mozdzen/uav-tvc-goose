#pragma once

#include <cmath>

struct Vector {
	float x, y, z;

	constexpr Vector();
	constexpr Vector(float, float, float);

	constexpr Vector operator*(const float) const;
	constexpr Vector operator/(const float) const;
	constexpr Vector operator+(const Vector &) const;
	constexpr Vector operator-(const Vector &) const;
	constexpr Vector operator^(const Vector &) const;
	constexpr float operator*(const Vector &) const;

	constexpr void normalize();

	constexpr float getLength() const;
	constexpr Vector getNormalized() const;

	static constexpr Vector X();
	static constexpr Vector Y();
	static constexpr Vector Z();
};

constexpr Vector::Vector() : x{0}, y{0}, z{0} {

}

constexpr Vector::Vector(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z} {

}

constexpr Vector Vector::operator*(const float num) const {
	return Vector(x*num, y*num, z*num);
}

constexpr Vector Vector::operator/(const float num) const {
	return Vector(x/num, y/num, z/num);
}

constexpr Vector Vector::operator+(const Vector &vec) const {
	return Vector(x + vec.x, y + vec.y, z + vec.z);
}

constexpr Vector Vector::operator-(const Vector &vec) const {
	return Vector(x - vec.x, y - vec.y, z - vec.z);
}

constexpr Vector Vector::operator^(const Vector &vec) const {
	return Vector(
		y*vec.z - z*vec.y,
		-(x*vec.z - z*vec.x),
		x*vec.y - y*vec.x
	);
}

constexpr float Vector::operator*(const Vector &vec) const {
	return (x*vec.x + y*vec.y + z*vec.z);
}

constexpr void Vector::normalize() {
	const float len = getLength();

	if(len<0.01f)
		return;

	x /=len;
	y /=len;
	z /=len;
}

constexpr float Vector::getLength() const {
	return std::sqrt(x*x + y*y + z*z);
}

constexpr Vector operator*(const float num, const Vector &vec) {
	return Vector(num*vec.x, num*vec.y, num*vec.z);
}

constexpr Vector operator-(const Vector &vec) {
	return Vector(-vec.x, -vec.y, -vec.z);
}

constexpr Vector Vector::X() {
	return Vector(1, 0, 0);
}

constexpr Vector Vector::Y() {
	return Vector(0, 1, 0);
}

constexpr Vector Vector::Z() {
	return Vector(0, 0, 1);
}

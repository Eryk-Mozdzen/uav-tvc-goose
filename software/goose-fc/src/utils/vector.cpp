#include "vector.h"
#include <cmath>

const Vector Vector::X(1, 0, 0);
const Vector Vector::Y(0, 1, 0);
const Vector Vector::Z(0, 0, 1);

Vector::Vector() : x{0}, y{0}, z{0} {

}

Vector::Vector(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z} {

}

Vector Vector::operator*(const float num) const {
	return Vector(x*num, y*num, z*num);
}

Vector Vector::operator/(const float num) const {
	return Vector(x/num, y/num, z/num);
}

Vector Vector::operator+(const Vector &vec) const {
	return Vector(x + vec.x, y + vec.y, z + vec.z);
}

Vector Vector::operator-(const Vector &vec) const {
	return Vector(x - vec.x, y - vec.y, z - vec.z);
}

Vector Vector::operator^(const Vector &vec) const {
	return Vector(
		y*vec.z - z*vec.y,
		-(x*vec.z - z*vec.x),
		x*vec.y - y*vec.x
	);
}

float Vector::operator*(const Vector &vec) const {
	return (x*vec.x + y*vec.y + z*vec.z);
}

void Vector::normalize() {
	const float len = getLength();

	if(len<0.01f)
		return;

	x /=len;
	y /=len;
	z /=len;
}

float Vector::getLength() const {
	return std::sqrt(x*x + y*y + z*z);
}

Vector Vector::getNormalized() const {
	const float len = getLength();

	if(len<0.01f)
		return Vector(x, y, z);

	return Vector(x/len, y/len, z/len);
}

Vector operator*(const float num, const Vector &vec) {
	return Vector(num*vec.x, num*vec.y, num*vec.z);
}

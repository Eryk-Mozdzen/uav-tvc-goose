#include "vector.h"

Vector::Vector() : x{0}, y{0}, z{0} {

}

Vector::Vector(float _x, float _y, float _z) : x{_x}, y{_y}, z{_z} {

}

Vector::Vector(const Matrix<3, 1> &mat) : x{mat(0, 0)}, y{mat(1, 0)}, z{mat(2, 0)} {

}

Vector::operator Matrix<3, 1>() const {
	return {x, y, z};
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

Vector operator*(const Matrix<3, 3> &mat, const Vector &vec) {
	return Vector(
		mat(0, 0)*vec.x + mat(0, 1)*vec.y + mat(0, 2)*vec.z,
		mat(1, 0)*vec.x + mat(1, 1)*vec.y + mat(1, 2)*vec.z,
		mat(2, 0)*vec.x + mat(2, 1)*vec.y + mat(2, 2)*vec.z
	);
}

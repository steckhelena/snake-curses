#include <cmath>

#include "utils.hpp"

Vector2D::Vector2D() {
	this->x = 0;
	this->y = 0;
}

Vector2D::Vector2D(double x, double y) {
	this->x = x;
	this->y = y;
}

float Vector2D::norm() {
	return std::sqrt(std::pow(this->x, 2.0) + std::pow(this->y, 2.0));
}

Vector2D Vector2D::operator -(const Vector2D other) {
	return Vector2D(this->x - other.x, this->y - other.y);
}

Vector2D Vector2D::operator +(const Vector2D other) {
	return Vector2D(this->x + other.x, this->y + other.y);
}
Vector2D Vector2D::operator -() {
	return Vector2D(-this->x, -this->y);
}

bool Vector2D::operator==(const Vector2D &other) {
	return (this->x == other.x) && (this->y == other.y);
}

bool Vector2D::operator==(const Vector2D *other) {
	return *this == *other;
}

bool operator==(const Vector2D &a, const Vector2D &b) {
	return (a.x==b.x && a.y==b.y);
}

bool Vector2D::operator!=(const Vector2D &other) {
	return !(*this == other);
}

bool Vector2D::operator!=(const Vector2D *other) {
	return !(*this == *other);
}

Vector2D Vector2D::operator/(const float scalar) {
	this->x /= scalar;
	this->y /= scalar;
	return *this;
}

Vector2D operator*(const float scalar, Vector2D vector) {
	vector.x *= scalar;
	vector.y *= scalar;
	return vector;
}

Vector2D operator*(Vector2D vector, const float scalar) {
	vector.x *= scalar;
	vector.y *= scalar;
	return vector;
}


std::ostream& operator<<(std::ostream &strm, Vector2D a) {
  return strm << "Vector2D(" << a.x << ", " << a.y << ")";
}

int sgn(int val) {
    return (0 < val) - (val < 0);
}

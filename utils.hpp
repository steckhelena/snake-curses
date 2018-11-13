#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#define WALL_CHAR '#'
#define MAX_SCORE 10
#define MIN_SCORE -3

class Vector2D {
	public:
		double x, y;
		Vector2D();
		Vector2D(double x, double y);

		float norm();

		Vector2D operator -(const Vector2D other);
		Vector2D operator +(const Vector2D other);
		Vector2D operator -();
		bool operator==(const Vector2D &other);
		bool operator==(const Vector2D *other);
		bool operator!=(const Vector2D &other);
		bool operator!=(const Vector2D *other);
		Vector2D operator/(const float);

		friend bool operator==(const Vector2D &a, const Vector2D &b);

		friend Vector2D operator*(const float scalar, Vector2D vector);
		friend Vector2D operator*(Vector2D vector, const float scalar);

		friend std::ostream& operator<<(std::ostream &strm, Vector2D a);
		friend std::istream& operator>>(std::istream &strm, Vector2D &a);
};


int sgn(int val);

#endif //UTILS_HPP

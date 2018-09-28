#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>

#define WALL_CHAR '#'
#define SNAKE_CHAR "O"
#define FOOD_CHAR "X"
#define MAX_SCORE 10

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
};

Vector2D operator*(const float scalar, Vector2D vector);

Vector2D operator*(Vector2D vector, const float scalar);

std::ostream& operator<<(std::ostream &strm, Vector2D a);

int sgn(int val);

#endif //UTILS_HPP

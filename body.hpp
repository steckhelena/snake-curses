// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <string>

#include "utils.hpp"

class Body {
	private:
		Vector2D speed;
		Vector2D position;

		char frame;
		unsigned int color;

	public:
		Body(Vector2D speed, Vector2D position, char frame, unsigned int color);

		Vector2D getPosition();
		void setPosition(Vector2D position);

		Vector2D getSpeed();
		void setSpeed(Vector2D speed);

		char getFrame();
		unsigned char getColor();

		friend std::ostream& operator<<(std::ostream &strm, const Body &a);
		friend std::istream& operator>>(std::istream &strm, Body &a);
};

class BodyList {
	protected:
		std::vector<Body *> bodies;

	public:
		~BodyList();

		void addBody(Body &c);
		void append(BodyList &c);

		void removeAt(int i);
		void clear();

		std::vector<Body*> &getBodies();

		friend std::ostream& operator<<(std::ostream &strm, const BodyList &a);
		friend std::istream& operator>>(std::istream &strm, BodyList &a);
};

#endif //BODY_HPP

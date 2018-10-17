#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "utils.hpp"
#include "body.hpp"

#define SNAKE_CHAR 'O'

class Snake: public BodyList {
	public:
		Snake(Vector2D position, Vector2D speed, int size, unsigned char color);
		Vector2D getHeadSpeed();
		void setHeadSpeed(Vector2D new_speed);
		Vector2D getHeadPosition();
		void grow();
};
		

#endif //SNAKE_HPP

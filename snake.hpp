#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "utils.hpp"
#include "body.hpp"

class Snake {
	private:
		BodyList *nodes;
		BodyFrames *frames;
	
	public:
		Snake(Vector2D position, Vector2D speed, int size);
		Vector2D getHeadSpeed();
		void setHeadSpeed(Vector2D new_speed);
		Vector2D getHeadPosition();
		void grow(int size);
		BodyList *getBodyList();
};
		

#endif //SNAKE_HPP

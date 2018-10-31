#include <vector>

#include "snake.hpp"
#include "utils.hpp"

Snake::Snake(Vector2D position, Vector2D speed, int size, unsigned char color) {
	for (int i=0; i<size; i++) {
		Vector2D pos(position.x, position.y + i);
		Body snake_part(speed, pos, SNAKE_CHAR, color);
		this->addBody(snake_part);
	}
}

Vector2D Snake::getHeadSpeed() {
	return this->getBodies().front()->getSpeed();
}

Vector2D Snake::getHeadPosition() {
	return this->getBodies().front()->getPosition();
}

void Snake::setHeadSpeed(Vector2D new_speed) {
	this->getBodies().front()->setSpeed(new_speed);
}

void Snake::grow() {
	Body *last = this->getBodies().back();
	Vector2D delta_position(last->getPosition());
	if (last->getSpeed().x != 0) {
		delta_position.x -= sgn(last->getSpeed().x);
	} else if (last->getSpeed().y != 0) {
		delta_position.y -= sgn(last->getSpeed().y);
	}
	Body snake_part(last->getSpeed(), delta_position, SNAKE_CHAR, last->getColor());
	this->addBody(snake_part);
}

void Snake::shrink() {
	Vector2D head_speed(this->getHeadSpeed());
	this->removeAt(0);
	this->setHeadSpeed(head_speed);
}

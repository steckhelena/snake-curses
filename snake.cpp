#include <vector>

#include "snake.hpp"
#include "utils.hpp"

Snake::Snake(Vector2D position, Vector2D speed, int size){
	this->frames = new BodyFrames(new BodyFrame(0, 0, SNAKE_CHAR));

	this->nodes = new BodyList();
	for (int i=0; i<size; i++) {
		Vector2D pos(position.x, position.y + i);
		nodes->addBody(new Body(speed, pos, this->frames));
	}
}


Vector2D Snake::getHeadSpeed() {
	return this->nodes->getBodies()->front()->getSpeed();
}

Vector2D Snake::getHeadPosition() {
	return this->nodes->getBodies()->front()->getPosition();
}

BodyList *Snake::getBodyList() {
	return this->nodes;
}

void Snake::setHeadSpeed(Vector2D new_speed) {
	this->nodes->getBodies()->front()->setSpeed(new_speed);
}

void Snake::grow(int size) {
	for (int i=0; i<size; i++) {
		Body *last = this->nodes->getBodies()->back();
		Vector2D delta_position(last->getPosition());
		if (last->getSpeed().x != 0) {
			delta_position.x -= sgn(last->getSpeed().x);
		} else if (last->getSpeed().y != 0) {
			delta_position.y -= sgn(last->getSpeed().y);
		}
		nodes->addBody(new Body(last->getSpeed(), delta_position, this->frames));
	}
}

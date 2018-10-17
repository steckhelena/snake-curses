// Autor: Marco Antonio Steck Filho - RA:183374
#include <algorithm>
#include <vector>
#include <cmath>

#include "ncurses.h"

// Private includes
#include "body.hpp"
#include "physics.hpp"
#include "utils.hpp"

Physics::Physics(Snake *snake, Food *food, int maxFood, int maxX, int maxY) {
	this->win = false;
	this->lost = false;
	this->score = 0;
	movement_blocked = false;
	this->food = food;
	this->ate_food = false;
	this->max_food = maxFood;
	this->snake = snake;
	this->normSpeed = snake->getHeadSpeed().norm();
	lastDelta = this->snake->getHeadPosition();
	this->maxX = maxX;
	this->maxY = maxY;
}


void Physics::update(float deltaT) {
	if (this->win || this->lost) {
		this->ate_food = false;
		return;
	}

	Vector2D speed = this->snake->getHeadSpeed();
	lastDelta = lastDelta + (speed*deltaT)/1000.0f;
	Vector2D delta_position = lastDelta - this->snake->getHeadPosition();
	if (std::abs(delta_position.x) >= 1 || std::abs(delta_position.y) >= 1) {
		this->movement_blocked = false;
		delta_position = Vector2D((int) delta_position.x, (int) delta_position.y);
		vector<Body*> &b = this->snake->getBodies();
		Vector2D ahead_position = delta_position + this->snake->getHeadPosition();
		Vector2D ahead_speed = this->snake->getHeadSpeed();
		for (Body *body: b) {
			Vector2D this_last_position = body->getPosition();
			Vector2D this_last_speed = body->getSpeed();
			body->setPosition(ahead_position);
			body->setSpeed(ahead_speed);
			ahead_position = this_last_position;
			ahead_speed = this_last_speed;
		}
		lastDelta = this->snake->getHeadPosition();

		if ((int)lastDelta.x <= -1 || (int) lastDelta.x >= this->maxX-1 ||\
			   	(int)lastDelta.y <= -1 || (int)lastDelta.y >= this->maxY-1) {
			this->lost = true;
		}

		vector<Vector2D> positions;
		for (Body *body: b) {
			Vector2D current = body->getPosition();
			for (Vector2D pos: positions) {
				if (pos == current) {
					this->lost = true;
					return;
				}
			}
			positions.push_back(current);
		}

		int i = 0;
		this->ate_food = false;
		for (Body *body: this->food->getBodies()) {
			if (Vector2D((int)lastDelta.x, (int)lastDelta.y) == \
					Vector2D((int)body->getPosition().x, (int)body->getPosition().y)) {
				this->snake->grow();
				this->ate_food = true;
				break;
			}
			i++;
		}
		if (this->ate_food) {
			this->food->despawnIndex(i);
			this->score++;
		}

		if (this->score >= MAX_SCORE) {
			this->win = true;
		}

		if (this->food->getNumFood() < this->max_food) {
			this->food->spawn();
		}
	}
}

void Physics::goUp() {
	if (this->snake->getHeadSpeed() != Vector2D(0, this->normSpeed) && !this->movement_blocked) {
		this->snake->setHeadSpeed(Vector2D(0, -this->normSpeed));
		this->movement_blocked = true;	
	}
	lastDelta = this->snake->getHeadPosition();
}

void Physics::goDown() {
	if (this->snake->getHeadSpeed() != Vector2D(0, -this->normSpeed) && !this->movement_blocked) {
		this->snake->setHeadSpeed(Vector2D(0, this->normSpeed));
		this->movement_blocked = true;	
	}
	lastDelta = this->snake->getHeadPosition();
}

void Physics::goLeft() {
	if (this->snake->getHeadSpeed() != Vector2D(this->normSpeed, 0) && !this->movement_blocked) {
		this->snake->setHeadSpeed(Vector2D(-this->normSpeed, 0));
		this->movement_blocked = true;	
	}
	lastDelta = this->snake->getHeadPosition();
}

void Physics::goRight() {
	if (this->snake->getHeadSpeed() != Vector2D(-this->normSpeed, 0) && !this->movement_blocked) {
		this->snake->setHeadSpeed(Vector2D(this->normSpeed, 0));	
		this->movement_blocked = true;	
	}
	lastDelta = this->snake->getHeadPosition();
}

bool Physics::didWin() {
	return this->win;
}

bool Physics::didLose() {
	return this->lost;
}

bool Physics::didEat() {
	return this->ate_food;
}

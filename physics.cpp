// Autor: Marco Antonio Steck Filho - RA:183374

// Private includes
#include "physics.hpp"

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

	// Spawns new food if the number of food is less than max
	while (this->food->getNumFood() < this->max_food) {
		this->food->spawn();
	}
}


void Physics::update(float deltaT) {
	// Do not run if game is over
	if (this->win || this->lost) {
		this->ate_food = false;
		return;
	}

	// Resets eat status
	this->ate_food = false;

	// Updates position
	Vector2D speed = this->snake->getHeadSpeed();
	lastDelta = lastDelta + (speed*deltaT)/1000.0f;
	Vector2D delta_position = lastDelta - this->snake->getHeadPosition();
	
	// If significant position change updates snake
	if (std::abs(delta_position.x) >= 1 || std::abs(delta_position.y) >= 1) {
		// Enables movement again and dequeues until it is blocked again
		this->movement_blocked = false;
		while (!this->action_queue.empty() && !this->movement_blocked) {
			this->action_queue.front()();
			this->action_queue.pop();
		}
		
		// Updates position of all snake parts
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
		// Resets delta so it doesnt accumulate forever
		// May slow down snake if the loop takes too long
		lastDelta = this->snake->getHeadPosition();
		
		// Checks if snake is in boundaries
		if (lastDelta.x <= 0 || (int) lastDelta.x >= this->maxX ||\
			   	lastDelta.y <= 0 || (int)lastDelta.y >= this->maxY) {
			this->lost = true;
		}
		
		// Checks if snake didn't collide with itself.
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
		
		// Checks if snake ate food
		int i = 0;
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
			this->food->spawn();
		}
		
		// Checks if snake hit max score
		if (this->score >= MAX_SCORE) {
			this->win = true;
		}
	}
}

void Physics::goUp() {
	// Tries to register the movement, if it cant enqueues it so it can be registered eventually
	if (!this->movement_blocked) {
		if (this->snake->getHeadSpeed() != Vector2D(0, this->normSpeed) && 
				this->snake->getHeadSpeed() != Vector2D(0, -this->normSpeed)) {
			this->snake->setHeadSpeed(Vector2D(0, -this->normSpeed));
			this->movement_blocked = true;
		}
	} else {
		this->action_queue.push(std::bind(&Physics::goUp, this));
	}
}

void Physics::goDown() {
	// Tries to register the movement, if it cant enqueues it so it can be registered eventually
	if (!this->movement_blocked) {
		if (this->snake->getHeadSpeed() != Vector2D(0, this->normSpeed) && 
				this->snake->getHeadSpeed() != Vector2D(0, -this->normSpeed)) {
			this->snake->setHeadSpeed(Vector2D(0, this->normSpeed));
			this->movement_blocked = true;
		}
	} else {
		this->action_queue.push(std::bind(&Physics::goDown, this));
	}
}

void Physics::goLeft() {
	// Tries to register the movement, if it cant enqueues it so it can be registered eventually
	if (!this->movement_blocked) {
		if (this->snake->getHeadSpeed() != Vector2D(-this->normSpeed, 0) && 
				this->snake->getHeadSpeed() != Vector2D(this->normSpeed, 0)) {
			this->snake->setHeadSpeed(Vector2D(-this->normSpeed, 0));	
			this->movement_blocked = true;
		}
	} else {
		this->action_queue.push(std::bind(&Physics::goLeft, this));
	}
}

void Physics::goRight() {
	// Tries to register the movement, if it cant enqueues it so it can be registered eventually
	if (!this->movement_blocked) {
		if (this->snake->getHeadSpeed() != Vector2D(-this->normSpeed, 0) && 
				this->snake->getHeadSpeed() != Vector2D(this->normSpeed, 0)) {
			this->snake->setHeadSpeed(Vector2D(this->normSpeed, 0));	
			this->movement_blocked = true;
		}
	} else {
		this->action_queue.push(std::bind(&Physics::goRight, this));
	}
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

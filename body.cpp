// Autor: Marco Antonio Steck Filho - RA:183374

// Built-in includes
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>

// Private includes
#include "body.hpp"
#include "utils.hpp"

using namespace std;

BodyFrame::BodyFrame(int pivot_x, int pivot_y, vector<string> frame) {
	this->pivot_x = pivot_x;
	this->pivot_y = pivot_y;

	this->frame = frame;
}

BodyFrame::BodyFrame(int pivot_x, int pivot_y, string frame) {
	this->pivot_x = pivot_x;
	this->pivot_y = pivot_y;

	istringstream iss(frame);
	for (string line; getline(iss, line); ) {
		this->frame.push_back(line);
	}
}

BodyFrame::BodyFrame(const BodyFrame &bf) {
	pivot_x = bf.pivot_x;
	pivot_y = bf.pivot_y;
	frame = bf.frame;
}

vector<string> BodyFrame::getFrame() {
	return this->frame;
}

int BodyFrame::getPivotX() {
	return this->pivot_x;
}

int BodyFrame::getPivotY() {
	return this->pivot_y;
}

BodyFrames::BodyFrames(BodyFrame *up, BodyFrame *down, BodyFrame *left, BodyFrame *right) {
	this->up = up;
	this->down = down;
	this->left = left;
	this->right = right;
}

BodyFrames::BodyFrames(BodyFrame *any) : BodyFrames(any, any, any, any) {}

BodyFrame *BodyFrames::getUp() {
	return this->up;
}

BodyFrame *BodyFrames::getDown() {
	return this->down;
}

BodyFrame *BodyFrames::getLeft() {
	return this->left;
}

BodyFrame *BodyFrames::getRight() {
	return this->right;
}

Body::Body(Vector2D speed, Vector2D position, BodyFrames *frames) {
	this->speed = speed;
	this->position = position;
	this->frames = frames;
}

void Body::setPosition(Vector2D new_position) {
	this->position = new_position;
}

void Body::setSpeed(Vector2D new_speed) {
	this->speed = new_speed;
}

Vector2D Body::getPosition() {
	return this->position;
}

Vector2D Body::getSpeed() {
	return this->speed;
}

BodyFrames *Body::getFrames() {
	return this->frames;
}

BodyFrame *Body::getCurrentFrame() {
	if (abs(this->speed.x) > abs(this->speed.y)) {
		if (this->speed.x > 0) {
			return this->frames->getRight();
		} else {
			return this->frames->getLeft();
		}
	} else {
		if (this->speed.y < 0) {
			return this->frames->getDown();
		} else {
			return this->frames->getUp();
		}
	}
}

BodyList::BodyList() {
	this->bodies = new std::vector<Body *>(0);
}

void BodyList::hardCopy(BodyList *ldc) {
	std::vector<Body *> *corpos = ldc->getBodies();

	for (auto const& body: *corpos) {
		Body *c = new Body(body->getPosition(), body->getSpeed(), body->getFrames());
		this->addBody(c);
	}
}

void BodyList::addBody(Body *c) {
	(this->bodies)->push_back(c);
}

void BodyList::removeBody(Body *c) {
	auto i = this->bodies->begin();
	for (Body *body: *this->bodies) {
		if (body == c) {
			break;
		}
		i++;
	}
	this->bodies->erase(i);
}

std::vector<Body*> *BodyList::getBodies() {
	return (this->bodies);
}

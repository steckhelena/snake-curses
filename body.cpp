// Autor: Marco Antonio Steck Filho - RA:183374

// Built-in includes
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

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

Body::Body(Vector2D speed, Vector2D position, BodyFrame frame) : frame(frame){
	this->speed = speed;
	this->position = position;
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

BodyFrame Body::getFrame() {
	return this->frame;
}

BodyList::BodyList() {
	this->bodies = new std::vector<Body *>(0);
}

void BodyList::hardCopy(BodyList *ldc) {
	std::vector<Body *> *corpos = ldc->getBodies();

	for (auto const& body: *corpos) {
		Body *c = new Body(body->getPosition(), body->getSpeed(), body->getFrame());
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

#include <cstdlib>
#include <ctime>

#include "ncurses.h"

#include "food.hpp"

Food::Food(int value, int maxX, int maxY): frame(0, 0, FOOD_CHAR) {
	this->maxX = maxX;
	this->maxY = maxY;
	this->value = value;
	this->food = new BodyList();
	this->n_food = 0;
	std::srand(std::time(NULL));
	spawn();
}

void Food::spawn() {
	int x, y;
	do {
	x = 1 + std::rand()%(this->maxX-2);
	y = 1 + std::rand()%(this->maxY-2);
	} while ((mvinch(y, x) & A_CHARTEXT) != ' ');
	
	this->n_food++;
	this->food->addBody(new Body(Vector2D(0,0), Vector2D(x, y), this->frame));
}

void Food::despawnIndex(int i) {
	this->n_food -= 1;
	this->food->getBodies()->erase(this->food->getBodies()->begin()+i);
}

int Food::getValue() {
	return this->value;
}

BodyList *Food::getBodyList() {
	return this->food;
}

int Food::getNumFood() {
	return this->n_food;
}

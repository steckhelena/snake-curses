#ifndef FOOD_HPP
#define FOOD_HPP

#include "body.hpp"
#include "utils.hpp"

class Food {
	private:
		int value;
		int n_food;

		BodyFrames *frames;
		BodyList *food;

		int maxX, maxY;

	public:
		Food(int value, int maxX, int maxY);
		void spawn();
		void despawnIndex(int i);
		int getValue();
		BodyList *getBodyList();
		int getNumFood();
};


#endif //FOOD_HPP

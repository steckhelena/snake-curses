#ifndef FOOD_HPP
#define FOOD_HPP

#include "body.hpp"
#include "utils.hpp"

#define FOOD_CHAR 'X'

class Food: public BodyList {
	private:
		unsigned char color;

		int maxX, maxY;
	public:
		Food(int maxX, int maxY, unsigned char color);
		void spawn();
		void despawnIndex(int i);
		int getNumFood();
};


#endif //FOOD_HPP

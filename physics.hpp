// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include "body.hpp"
#include "food.hpp"
#include "snake.hpp"

class Physics {
	private:
		Food *food;
		Snake *snake;

		int max_food;
		bool ate_food;

		float normSpeed;
		Vector2D lastDelta;

		bool win;
		bool lost;

		bool movement_blocked;

		int maxX;
		int maxY;

		int score;

	public:
		Physics(Snake *s, Food *food, int max_food, int maxX, int maxY);
		void update(float deltaT);

		void goUp();
		void goDown();
		void goLeft();
		void goRight();

		bool didWin();
		bool didLose();
		bool didEat();
};

#endif //PHYSICS_HPP

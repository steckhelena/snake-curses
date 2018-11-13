// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef PHYSICS_HPP
#define PHYSICS_HPP

#include <algorithm>
#include <vector>
#include <cmath>
#include <functional>

#include "ncurses.h"
#include <queue>

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
		std::queue<std::function<void()>> action_queue;

		int maxX;
		int maxY;

		int score;

	public:
		Physics(Snake *s, Food *food, int max_food, int maxX, int maxY);
		void update(float deltaT);
		void collide();
		void resetDeltas();
		void setLoss();
		void setWin();

		void goUp();
		void goDown();
		void goLeft();
		void goRight();

		bool didWin();
		bool didLose();
		bool didEat();
};

#endif //PHYSICS_HPP

// Autor: Marco Antonio Steck Filho - RA:183374

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "audio.hpp"
#include "body.hpp"
#include "food.hpp"
#include "physics.hpp"
#include "snake.hpp"
#include "teclado.hpp"
#include "tela.hpp"
#include "utils.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {
	Audio::Sample *on_food;
	on_food = new Audio::Sample();
	on_food->load("assets/blip.dat");

	Audio::Player *player;
	player = new Audio::Player();
	player->init();

	Tela *tela = new Tela();
	tela->init();

	Teclado *teclado = new Teclado();
	teclado->init();

	Snake *snake = new Snake(Vector2D(tela->getMaxX()/2, tela->getMaxY()/2), Vector2D(0,-10), 5);
	Food *food = new Food(1, tela->getMaxX(), tela->getMaxY());

	tela->appendList(snake->getBodyList());
	tela->appendList(food->getBodyList());

	Physics *physics = new Physics(snake, food, 1, tela->getMaxX(), tela->getMaxY());

	uint64_t t0, t1, deltaT;

	int i = 0;

	t1 = get_now_ms();
	while (1) {
		// Atualiza timers
		t0 = t1;
		t1 = get_now_ms();
		deltaT = t1-t0;

		// Atualiza fisica
		physics->update(deltaT);

		// Atualiza tela
		if (physics->didWin()) {
			tela->showText("Congratz M8!!");
		} else if (physics->didLose()) {
			tela->showText("You lost M8!!");
		} else {
			tela->update();
		}

		// Toca audio ao comer
		if (physics->didEat()) {
			on_food->set_position(0);
			player->play(on_food);
			on_food->reverse();
		}

		// Lê o teclado
		char c = teclado->getchar();
		if (c=='w') {
			physics->goUp();
		} else if (c=='a') {
			physics->goLeft();
		} else if (c=='s') {
			physics->goDown();
		} else if (c=='d') {
			physics->goRight();
		} else if (c=='q') {
			break;
		}

		std::this_thread::sleep_for (std::chrono::milliseconds(100));
		i++;
	}

	tela->stop();
	teclado->stop();
	return 0;
}

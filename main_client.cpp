// Autor: Marco Antonio Steck Filho - RA:183374

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "audio.hpp"
#include "body.hpp"
#include "food.hpp"
#include "keyboard.hpp"
#include "snake_sockets.hpp"
#include "snake.hpp"
#include "tela.hpp"
#include "utils.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {
	/*
	Audio::Sample *on_food;
	on_food = new Audio::Sample();
	on_food->load("assets/blip.dat");

	Audio::Player *player;
	player = new Audio::Player();
	player->init();
	*/

	Tela *tela = new Tela();
	tela->init();

	SnakeSockets::SnakeClient client;
	client.init("127.0.0.1");

	BodyList *everything = new BodyList();
	BodyList *snake = new BodyList();

	tela->appendList(everything);
	
	while (client.isAlive()) {
		client.updateBodies(everything);
		client.updateTarget(snake);

		tela->update(snake);

		std::this_thread::sleep_for (std::chrono::milliseconds(1));
	}

	return 0;
}

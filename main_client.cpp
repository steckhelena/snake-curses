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
	Audio::Sample *on_food;
	on_food = new Audio::Sample();
	on_food->load("assets/blip.dat");

	Audio::Player *player;
	player = new Audio::Player();
	player->init();

	Tela *tela = new Tela();
	tela->init();

	SnakeSockets::SnakeClient client;
	if (!client.init(tela->showPrompt("Enter server ip address: "))) {
		delete tela;
		std::cerr << "Couldn't stabilish connection to server, are you sure the ip is correct?" << std::endl;
		exit(-1);
	}

	tela->showText("Connected! Waiting for other clients!");

	BodyList *everything = new BodyList();
	BodyList *snake = new BodyList();

	tela->appendList(everything);

	client.updateBodiesAndTarget(everything, snake);

	tela->setMaxX(client.getMaxX());
	tela->setMaxY(client.getMaxY());

	while (client.isAlive()) {
		client.updateBodiesAndTarget(everything, snake);
		bool ate = client.didEat();

		tela->update(snake);
		if (ate) {
			on_food->set_position(0);
			player->play(on_food);
			on_food->reverse();
		}

		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}

	delete everything;
	delete snake;
	delete tela;

	return 0;
}

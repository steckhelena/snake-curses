// Autor: Marco Antonio Steck Filho - RA:183374

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>

#include "audio.hpp"
#include "body.hpp"
#include "food.hpp"
#include "keyboard.hpp"
#include "physics.hpp"
#include "snake.hpp"
#include "tela.hpp"
#include "utils.hpp"

using namespace std::chrono;
uint64_t get_now_ms() {
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

int main () {
	KeyboardClient *keyClient = new KeyboardClient("127.0.0.1");
	keyClient->init();

	while (keyClient->isAlive()) {
		std::this_thread::sleep_for (std::chrono::milliseconds(100));
	}

	keyClient->stop();
	return 0;
}

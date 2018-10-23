// Autor: Marco Antonio Steck Filho - RA:183374

#include <iostream>
#include <cmath>
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
	uint64_t t0, t1, deltaT;

	SnakeSockets::SnakeServer server(12.0, 1, 10, 500, 500, 1);
	server.init();

	std::cout << "All clients connected! Starting game!" << std::endl;

	t1 = get_now_ms();
	while (1) {
		// Atualiza timers
		t0 = t1;
		t1 = get_now_ms();
		deltaT = t1-t0;
		
		// Atualiza clientes
		server.update(deltaT);

		// Tries to keep the loop time constant
		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(10-(get_now_ms()-t1), (uint64_t)0)));
	}

	return 0;
}

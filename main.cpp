// Autor: Marco Antonio Steck Filho - RA:183374

#include <iostream>
#include <cmath>
#include <cstring>
#include <chrono>
#include <thread>
#include <vector>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>

#include "audio.hpp"
#include "body.hpp"
#include "food.hpp"
#include "keyboard.hpp"
#include "snake_sockets.hpp"
#include "snake.hpp"
#include "tela.hpp"
#include "utils.hpp"



static const int maxEndTime = 10000;

uint64_t get_now_ms() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

std::string selectIP() {
	struct ifaddrs *ifaddr, *ifa;
	int family, s;
	char host[NI_MAXHOST];

	if (getifaddrs(&ifaddr) == -1) {
		std::cerr << "Error getifaddrs: " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	int i = 1;
	std::cout << "The following interfaces are available: " << std::endl;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {
			s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
					host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
			if (s != 0) {
				std::cerr << "getnameinfo() failed: " << std::strerror(errno) << std::endl;
				exit(EXIT_FAILURE);
			}
			std::cout << i++ << ": (<Interface>: " << ifa->ifa_name << " \t <Address> " << host << ")" << std::endl;
		}
	}

	int selected;
	std::cout << "Please select the interface you want to use: (0 or numbers not listed to bind to all)" << std::endl;
	std::cin >> selected;

	i = 1;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) {
			if (i++ == selected) {
				s = getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in),
						host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
				if (s != 0) {
					std::cerr << "getnameinfo() failed: " << std::strerror(errno) << std::endl;
					exit(EXIT_FAILURE);
				}

				std::cout << "Selected: " << host << std::endl;

				return std::string(host);
			}
		}
	}

	return "";
}

int main () {
	uint64_t t0, t1, deltaT, timeEnd=0;
	uint64_t last_second;

	SnakeSockets::SnakeServer server(1.5, 10, 10, 100, 50, 2);
	if (server.init(selectIP())) {
		std::cout << "All clients connected! Starting game!" << std::endl;
	}
	t1 = get_now_ms();
	while (server.getConnectedClientsNumber() > 0) {
		// Atualiza timers
		t0 = t1;
		t1 = get_now_ms();
		deltaT = t1-t0;

		// Atualiza clientes
		if (!server.didEnd()) {
			server.update(deltaT);
		}

		if (timeEnd == 0 && server.didEnd()) {
			std::cout << "Game finished with at least one winner!" << std::endl;
			std::cout << "Waiting for clients to disconnect!" << std::endl;
			timeEnd = get_now_ms();
			last_second = (get_now_ms()-timeEnd)/1000;
		}

		if (server.didEnd() && last_second != (get_now_ms() - timeEnd)/1000) {
			std::cout << "Server forcibly closing in " << (maxEndTime/1000 - last_second) << " seconds!" << std::endl;
			last_second = (get_now_ms()-timeEnd)/1000;
		}

		if (server.didEnd() && (get_now_ms()-timeEnd) > maxEndTime) {
			break;
		}

		// Tries to keep the loop time constant
		std::this_thread::sleep_for(std::chrono::milliseconds(std::max(10-(get_now_ms()-t1), (uint64_t)0)));
	}

	if (server.getConnectedClientsNumber() > 0) {
		std::cout << "Forcibly closing: " << server.getConnectedClientsNumber() << " clients!" << std::endl;
		server.forceShutdown();
	}

	return 0;
}

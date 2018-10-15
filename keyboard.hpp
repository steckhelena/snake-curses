// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef TECLADO_HPP
#define TECLADO_HPP

// Built-in includes
#include <vector>
#include <chrono>
#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Third-party includes
#include <ncurses.h>

#define KEYBOARD_PORT 1337

class KeyboardServer {
	private:
		char last_capture;
		volatile bool running;

		std::thread kb_thread;

		void thread();

		int socket_fd;
		socklen_t client_size;
		int connection_fd;
		struct sockaddr_in myself, client;

	public:
		~KeyboardServer();
		void stop();
		bool init();
		char getchar();
};

class KeyboardClient {
	private:
		char last_capture;
		int running;

		void thread();

		int socket_fd;
		struct sockaddr_in target;

		std::thread kb_thread;

		std::string ip;

	public:
		KeyboardClient(std::string ip);
		~KeyboardClient();
		void stop();
		bool init();
		bool isAlive();
};

#endif //TECLADO_HPP

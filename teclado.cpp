// Autor: Marco Antonio Steck Filho - RA:183374

// Built-in includes
#include <vector>
#include <chrono>
#include <thread>

// Third-party includes
#include <ncurses.h>

// Private includes
#include "teclado.hpp"

void threadfun (char *keybuffer, int *control) {
	char c;
	while ((*control) == 1) {
		c = getch();
		if (c!=ERR) (*keybuffer) = c;
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	return;
}

Teclado::Teclado() {
}

Teclado::~Teclado() {
}

void Teclado::init() {
	raw();                  // Line buffering disabled
	keypad(stdscr, TRUE);   // We get F1, F2 etc...
	noecho();               // Don't echo() while we do getch
	curs_set(0);            // Do not display cursor
	nodelay(stdscr, TRUE);

	this->rodando = 1;
	std::thread newthread(threadfun, &(this->ultima_captura), &(this->rodando));
	(this->kb_thread).swap(newthread);
}

void Teclado::stop() {
	this->rodando = 0;
	(this->kb_thread).join();
}

char Teclado::getchar() {
	char c = this->ultima_captura;
	this->ultima_captura = 0;
	return c;
}

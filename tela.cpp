// Autor: Marco Antonio Steck Filho - RA:183374

// Standar includes
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>

// Third-party includes
#include <ncurses.h>

// Private includes
#include "body.hpp"
#include "tela.hpp"

void Tela::init() {
	initscr();			       /* Start curses mode 		*/
	raw();				         /* Line buffering disabled	*/
	curs_set(0);           /* Do not display cursor */

	// Gets screen sizes and adjusts them because it can get buggy in some terminals
	getmaxyx(stdscr, this->height, this->width);
	this->width -= 1;
	this->height -= 1;

	// Starts game color system
	if (has_colors() == TRUE) {
		start_color();
		init_pair(1, COLOR_RED, COLOR_BLACK);
		init_pair(2, COLOR_GREEN, COLOR_BLACK);
		init_pair(3, COLOR_YELLOW, COLOR_BLACK);
		init_pair(4, COLOR_BLUE, COLOR_BLACK);
		init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(6, COLOR_CYAN, COLOR_BLACK);
		init_pair(7, COLOR_WHITE, COLOR_BLACK);
		init_pair(8, COLOR_RED, COLOR_WHITE);
		init_pair(9, COLOR_GREEN, COLOR_WHITE);
		init_pair(10, COLOR_YELLOW, COLOR_WHITE);
		attron(A_BOLD);
		clear();
		color_set(1, NULL);
	}
}

void Tela::appendList(BodyList *ldc) {
	this->lista.push_back(ldc);
}

// Target list is the list which will be used for centering the cam
void Tela::update(BodyList *target) {
	if (target->getBodies().empty()) {
		return;
	}

	static unsigned int clear_interval = 0;

	// Erases all screen bodies
	if (clear_interval%1000) {
		clear();
	} else {
		erase();
	}

	clear_interval++;

	// Draws game borders relative to target
	makeBorder(target);

	// Calculates screen offsets based on first target body location
	int global_offset_x = target->getBodies().front()->getPosition().x;
	int global_offset_y = target->getBodies().front()->getPosition().y;

	// Desenha corpos na tela
	for (BodyList *list: this->lista) {
		std::vector<Body*> corpos = list->getBodies();

		for (auto &body: corpos) {
			int x = body->getPosition().x;
			int y = body->getPosition().y;
			char ch = body->getFrame();

			// Shifts all objects to be relative to main body, centering the camera
			// on that object
			x = (x - global_offset_x) + this->width/2;
			y = (y - global_offset_y) + this->height/2;

			// Prints object if inside the screen
			if (has_colors() == TRUE) {
				attron(COLOR_PAIR(body->getColor()));
				if ((x>=0 && x<this->width) && (y>=0 && y<this->height)) {
					move(y, x);
					addch(ch);
				}
				attroff(COLOR_PAIR(body->getColor()));
			} else {
				if ((x>=0 && x<this->width) && (y>=0 && y<this->height)) {
					move(y, x);
					addch(ch);
				}
			}
		}
	}

	// Atualiza tela
	refresh();
}

void Tela::showText(std::string text) {
	clear();
	mvprintw(this->height/2, (this->width/2)-text.length()/2, text.c_str());
	refresh();
}

void Tela::showTextBlocking(std::string text) {
	int c;
	bool waiting = true;
	while (waiting) {
		c = getch();
		switch(c) {
			case('\n'):
			case('\r'):
			case(KEY_ENTER):
				waiting=false;
				break;
			default:
				break;
		}
		clear();
		mvprintw(this->height/2, (this->width/2)-text.length()/2, text.c_str());
		std::string message = "(Press enter to close this message)";
		mvprintw(this->height/2+1, (this->width/2)-message.length()/2, message.c_str());
		refresh();
		std::this_thread::sleep_for (std::chrono::milliseconds(500));
	}
}

std::string Tela::showIpPrompt(std::string prompt) {
	clear();

	std::string message = "(Press enter when done)";
	mvprintw(this->height/2, (this->width/2)-prompt.length()/2, prompt.c_str());
	mvprintw(this->height/2 + 1, (this->width/2)-message.length()/2, message.c_str());

	int c;
	noecho();

	char buffer[16];
	buffer[0] = '\0';
	int i = 0;
	move(this->height/2 + 3, (this->width/2));
	bool reading = true;
	while(reading) {
		c = getch();
		switch(c) {
			case('\n'):
			case('\r'):
			case(KEY_ENTER):
				reading=false;
				break;
			case(127):
			case(KEY_DC):
			case(KEY_BACKSPACE):
				if (i != 0) {
					i--;
					buffer[i] = '\0';
				}
				break;
			case('0'):
			case('1'):
			case('2'):
			case('3'):
			case('4'):
			case('5'):
			case('6'):
			case('7'):
			case('8'):
			case('9'):
			case('.'):
				if (i < 15) {
					buffer[i++] = c;
					buffer[i] = '\0';
				}
				break;
			default:
				break;
		}
		move(this->height/2 + 3, 0);
		clrtoeol();
		mvprintw(this->height/2 + 3, (this->width/2)-i/2, buffer);
	}

	clear();
	return std::string(buffer);
}

void Tela::makeBorder(BodyList *target) {
	// Calculates screen offsets based on first target body location
	int global_offset_x = target->getBodies().front()->getPosition().x;
	int global_offset_y = target->getBodies().front()->getPosition().y;

	int x_left = (0 - global_offset_x) + this->width/2;
	int x_right = (this->max_x - global_offset_x) + this->width/2;

	int y_bot = (0 - global_offset_y) + this->height/2;
	int y_top = (this->max_y - global_offset_y) + this->height/2;

	for (int x=x_left; x<x_right; x++) {
		if (x > 0 && x < this->width) {
			if ((y_bot > 0 && y_bot < this->height)) {
				move(y_bot, x);
				addch(WALL_CHAR);
			}
			if (y_top > 0 && y_top < this->height) {
				move(y_top, x);
				addch(WALL_CHAR);
			}
		}
	}

	for (int y=y_bot; y<y_top; y++) {
		if (y > 0 && y < this->height) {
			if (x_left > 0 && x_left < this->width) {
				move(y, x_left);
				addch(WALL_CHAR);
			}
			if (x_right > 0 && x_right < this->width) {
				move(y, x_right);
				addch(WALL_CHAR);
			}
		}
	}
}

int Tela::getMaxWidth() {
	return this->width;
}

int Tela::getMaxHeight() {
	return this->height;
}

void Tela::setMaxX(int max_x) {
	this->max_x = max_x;
}

void Tela::setMaxY(int max_y) {
	this->max_y = max_y;
}

void Tela::stop() {
	endwin();
}

Tela::~Tela() {
	this->stop();;
}

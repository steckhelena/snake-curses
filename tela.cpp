// Autor: Marco Antonio Steck Filho - RA:183374

// Third-party includes
#include <ncurses.h>
#include <vector>
#include <iostream>

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
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	attron(A_BOLD);
	clear();
	color_set(1, NULL);
}

void Tela::appendList(BodyList *ldc) {
	this->lista.push_back(ldc);
}

// Target list is the list which will be used for centering the cam
void Tela::update(BodyList *target) {
	// Erases all screen bodies
	erase();

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
			if ((x>=0 && x<this->width) && (y>=0 && y<this->height)) {
				move(y, x);
				addch(ch);
			}
		}
	}

	// Atualiza tela
	refresh();
}

void Tela::showText(string text) {
	clear();
	mvprintw(this->height/2, (this->width/2)-text.length()/2, text.c_str());
	refresh();
}

void Tela::makeBorder(BodyList *target) {
	// Calculates screen offsets based on first target body location
	int global_offset_x = target->getBodies().front()->getPosition().x;
	int global_offset_y = target->getBodies().front()->getPosition().y;

	int x_left = (0 - global_offset_x) + this->width/2;
	int x_right = (this->width - global_offset_x) + this->width/2;

	int y_bot = (0 - global_offset_y) + this->height/2;
	int y_top = (this->height - global_offset_y) + this->height/2;

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

int Tela::getMaxX() {
	return this->width;
}

int Tela::getMaxY() {
	return this->height;
}

void Tela::stop() {
	endwin();
}

Tela::~Tela() {
	this->stop();;
}

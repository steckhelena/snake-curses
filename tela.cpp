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
	getmaxyx(stdscr, this->maxY, this->maxX);
	this->maxX -= 1;
	this->maxY -= 1;

	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
	attron(A_BOLD);
	clear();
	color_set(1, NULL);
}

void Tela::appendList(BodyList *ldc) {
	this->lista.push_back(ldc);
}

void Tela::update() {
	// Apaga corpos na tela
	erase();

	// Desenha uma caixa
	makeBorder();
	//wborder(stdscr, '|', '|', '_', '_', '*', '*', '*', '*');

	// Desenha corpos na tela
	for (BodyList *list: this->lista) {
		std::vector<Body *> *corpos = list->getBodies();

		for (auto const& body: *corpos) {
			int i = (int) (body->getPosition().x);
			int j = (int) (body->getPosition().y);

			BodyFrame *frame = body->getCurrentFrame();
			i -= frame->getPivotX();
			j -= frame->getPivotY();

			int x = i;
			int y = j;
			for (string& line: frame->getFrame()) {
				y++;
				for (char& ch: line) {
					x++;
					// Prints object if inside the screen
					if ((x>=0 && x<this->maxX) && (y>=0 && y<this->maxY)) {
						move(y, x);
						echochar(ch);
					}
				}
				x = i;
			}
		}
	}

	// Atualiza tela
	refresh();
}

void Tela::showText(string text) {
	clear();
	mvprintw(this->maxY/2, (this->maxX/2)-text.length()/2, text.c_str());
	refresh();
}

void Tela::makeBorder() {
	for (int x=0; x<this->maxX; x++) {
		move(0, x);
		echochar(WALL_CHAR);
		move(this->maxY, x);
		echochar(WALL_CHAR);
	}

	for (int y=0; y<this->maxY; y++) {
		move(y, 0);
		echochar(WALL_CHAR);
		move(y, this->maxX);
		echochar(WALL_CHAR);
	}
}

int Tela::getMaxX() {
	return this->maxX;
}

int Tela::getMaxY() {
	return this->maxY;
}

void Tela::stop() {
	endwin();
}

Tela::~Tela() {
	this->stop();;
}

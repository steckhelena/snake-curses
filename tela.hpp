// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef TELA_HPP
#define TELA_HPP

#include "body.hpp"

class Tela {
	private:
		std::vector<BodyList *> lista;
		int width, height;
		int max_x, max_y;
		void makeBorder(BodyList *target);

	public:
		~Tela();
		void stop();
		void init();
		void appendList(BodyList *ldc);
		void update(BodyList *target);
		void showText(string text);
		int getMaxX();
		int getMaxY();
};

#endif //TELA_HPP

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

		void showText(std::string text);
		void showTextBlocking(std::string text);
		std::string showIpPrompt(std::string prompt);

		int getMaxHeight();
		int getMaxWidth();

		void setMaxX(int max_x);
		void setMaxY(int max_y);
};

#endif //TELA_HPP

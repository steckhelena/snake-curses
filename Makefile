CXXFLAGS=-lncurses -lportaudio -lpthread -std=c++11

SRC=main.cpp \
	audio.cpp \
	body.cpp \
	physics.cpp \
	food.cpp \
	snake.cpp \
	teclado.cpp \
	tela.cpp \
	utils.cpp

model: $(SRC)
	g++ -osnake-curses $(SRC) $(CXXFLAGS)

debug: $(SRC)
	g++ -g -osnake-curses $(SRC) $(CXXFLAGS)

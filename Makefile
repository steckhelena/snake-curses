CC=g++

CXXFLAGS=-lncurses -lportaudio -lpthread -std=c++11

SERVER_EXE=snake-curses-server
CLIENT_EXE=snake-curses-client

COMMON_SRC=audio.cpp \
		   body.cpp \
		   physics.cpp \
		   food.cpp \
		   snake.cpp \
		   keyboard.cpp \
		   snake_sockets.cpp \
		   tela.cpp \
		   utils.cpp

SERVER_SRC=main.cpp \
		   $(COMMON_SRC)

CLIENT_SRC=main_client.cpp \
		   $(COMMON_SRC)

all: server client

debug: debug_server debug_client

server: $(SRC)
	@echo "Building server to \"$(SERVER_EXE)\"!"
	@$(CC) -o$(SERVER_EXE) $(SERVER_SRC) $(CXXFLAGS)

client: $(SRC)
	@echo "Building client to \"$(CLIENT_EXE)\"!"
	@$(CC) -o$(CLIENT_EXE) $(CLIENT_SRC) $(CXXFLAGS)

debug_server: $(SRC)
	@echo "Building debug server to \"$(SERVER_EXE)\"!"
	@$(CC) -g -o$(SERVER_EXE) $(SERVER_SRC) $(CXXFLAGS)

debug_client: $(SRC)
	@echo "Building debug server to \"$(CLIENT_EXE)\"!"
	@$(CC) -g -o$(CLIENT_EXE) $(CLIENT_SRC) $(CXXFLAGS)

.PHONY: clean

clean:
	@echo "Removing \"$(SERVER_EXE)\" and \"$(CLIENT_EXE)\"!"
	@rm -f $(SERVER_EXE) $(CLIENT_EXE)

// Autor: Marco Antonio Steck Filho - RA:183374

// Private includes
#include "keyboard.hpp"

/************************************************************************
 * KeyboardServer
 ************************************************************************/

void KeyboardServer::thread () {
	char c;

	while (this->running) {
		int msglen = recv(this->connection_fd, &c, 1, MSG_DONTWAIT);
		if (c != ERR && msglen > 0) {
			this->last_capture = c;
		}
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	return;
}

KeyboardServer::~KeyboardServer() {
	if (this->running) {
		this->stop();
	}
}

bool KeyboardServer::init() {
	// Initializing server socket
	this->client_size = (socklen_t)sizeof(this->client);
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	this->myself.sin_family = AF_INET;
	this->myself.sin_port = htons(KEYBOARD_PORT);
	this->myself.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_fd, (struct sockaddr*)&this->myself, sizeof(this->myself)) != 0) {
		return false;
	}

	// Listens for connections
	listen(this->socket_fd, 1);
	std::cout << "\nWaiting for client keyboard connections!\n" << std::endl;
	this->connection_fd = accept(socket_fd, (struct sockaddr*)&this->client, &this->client_size);

	// Launches server thread
	this->running = true;
	this->kb_thread = std::thread(&KeyboardServer::thread, this);

	return true;
}

void KeyboardServer::stop() {
	this->running = false;
	(this->kb_thread).join();
	close(this->connection_fd);
	close(this->socket_fd);
}

char KeyboardServer::getchar() {
	char c = this->last_capture;
	this->last_capture = 0;
	return c;
}


/************************************************************************
 * KeyboardClient
 ************************************************************************/

KeyboardClient::KeyboardClient(std::string ip) {
	this->ip = ip;
}

KeyboardClient::~KeyboardClient() {
	if (this->running) {
		this->stop();
	}
}

void KeyboardClient::thread () {
	char c, quit;
	while (this->running) {
		c = getch();
		if (c!=ERR) {
			int msglen = recv(this->socket_fd, &quit, 1, MSG_DONTWAIT);
			if (msglen == 0) {
				this->running = false;
			} else if (send(this->socket_fd, &c, 1, 0) == -1) {
				this->running = false;
			}
		}
		std::this_thread::sleep_for (std::chrono::milliseconds(10));
	}
	return;
}

bool KeyboardClient::isAlive() {
	return this->running;	
}

bool KeyboardClient::init() {
	initscr();
	raw();                  // Line buffering disabled
	keypad(stdscr, TRUE);   // We get F1, F2 etc...
	noecho();               // Don't echo() while we do getch
	curs_set(0);            // Do not display cursor
	nodelay(stdscr, TRUE);
	
	// Initializing server socket
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Trying to connect to server
	this->target.sin_family = AF_INET;
	this->target.sin_port = htons(KEYBOARD_PORT);
	inet_aton(this->ip.c_str(), &(target.sin_addr));
	if (connect(this->socket_fd, (struct sockaddr*)&this->target, sizeof(target)) != 0) {
		return false;
	}

	// Launches server thread
	this->running = true;
	this->kb_thread = std::thread(&KeyboardClient::thread, this);

	return true;
}

void KeyboardClient::stop() {
	this->running = false;
	(this->kb_thread).join();
	close(this->socket_fd);
	endwin();
}

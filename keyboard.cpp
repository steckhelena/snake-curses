// Autor: Marco Antonio Steck Filho - RA:183374

#include <cstring>

// Private includes
#include "keyboard.hpp"

/************************************************************************
 * Helpers
 ************************************************************************/

int getSocketError(int fd) {
   int err = 1;
   socklen_t len = sizeof err;
   if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len))
      std::cerr << "Fatal error: getSO_ERROR" << std::endl;
   if (err)
      errno = err;              // set errno to the socket SO_ERROR
   return err;
}

void closeSocket(int fd) {
	if (fd >= 0) {
		getSocketError(fd); // Limpa os erros que podem fazer o socket nao fechar
		if (shutdown(fd, SHUT_RDWR) < 0) {
			if (errno != ENOTCONN && errno != EINVAL) {
				std::cerr << "Error on socket shutdown: " << std::strerror(errno) << std::endl;
			}
		}
		if (close(fd) < 0) {// finally call close()
			std::cerr << "Error on socket shutdown: " << std::strerror(errno) << std::endl;
		}
	}
}

/************************************************************************
 * KeyboardServer
 ************************************************************************/

void KeyboardServer::thread () {
	char c;

	while (this->running) {
		int msglen = recv(this->connection_fd, &c, 1, MSG_DONTWAIT);
		this->mutex.lock();
		if (c != ERR && msglen > 0 && this->last_capture == 0) {
			this->last_capture = c;
		} else if (c != ERR && msglen == 0) {
			this->running = false;
		}
		this->mutex.unlock();
	}

	closeSocket(this->connection_fd);
	return;
}

KeyboardServer::~KeyboardServer() {
	if (this->running) {
		this->stop();
	}
	if (is_owner) {
		closeSocket(this->socket_fd);
	}
}

bool KeyboardServer::init() {
	this->is_owner = true;

	// Initializing server socket
	this->client_size = (socklen_t)sizeof(this->client);
	this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	this->myself.sin_family = AF_INET;
	this->myself.sin_port = htons(KEYBOARD_PORT);
	this->myself.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(this->socket_fd, (struct sockaddr*)&this->myself, sizeof(this->myself)) != 0) {
		std::cerr << "Error binding: " << std::strerror(errno) << std::endl;
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

bool KeyboardServer::init(int connection_fd) {
	this->is_owner = false;

	// Sets file descriptor
	this->connection_fd = connection_fd;

	// Launches server thread
	this->running = true;
	this->kb_thread = std::thread(&KeyboardServer::thread, this);

	return true;
}

void KeyboardServer::stop() {
	this->running = false;
	(this->kb_thread).join();
}

char KeyboardServer::getchar() {
	this->mutex.lock();
	char c = this->last_capture;
	this->last_capture = 0;
	this->mutex.unlock();
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
			if (c == 'q') {
				close(this->socket_fd);
				this->running = false;
				break;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
	closeSocket(this->socket_fd);
	endwin();
}

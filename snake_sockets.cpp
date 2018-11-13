// Autor: Marco Antonio Steck Filho - RA:183374

#include <cstring>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "snake_sockets.hpp"

/************************************************************************
 * Helpers
 ************************************************************************/

static int getSocketError(int fd) {
	int err = 1;
	socklen_t len = sizeof err;
	if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *)&err, &len))
		std::cerr << "Fatal error: getSO_ERROR" << std::endl;
	if (err)
		errno = err;              // set errno to the socket SO_ERROR
	return err;
}

static void closeSocket(int fd) {
	if (fd >= 0) {
		getSocketError(fd); // Limpa os erros que podem fazer o socket nao fechar
		if (shutdown(fd, SHUT_RDWR) < 0) {
			if (errno != ENOTCONN && errno != EINVAL) {
				std::cerr << "Error on socket shutdown: " << std::strerror(errno) << std::endl;
			}
		}
		// Waits to receive FIN
		char alo;
		while (recv(fd, &alo, 1, 0) > 0);
		if (close(fd) < 0) {// finally call close()
			std::cerr << "Error on socket shutdown: " << std::strerror(errno) << std::endl;
		}
	}
}

static int send_int(int fd, int num) {
	int32_t conv = htonl(num);
	char *data = (char*)&conv;
	int left = sizeof(conv);
	int rc;
	while (left > 0) {
		rc = send(fd, data, left, 0);
		if (rc < 0) {
			if (errno != EINTR) {
				return -1;
			}
		}
		else {
			data += rc;
			left -= rc;
		}
	}
	return 0;
}

static int receive_int(int fd, int *num) {
	int32_t ret;
	char *data = (char*)&ret;
	int left = sizeof(ret);
	int rc;
	while (left > 0) {
		rc = recv(fd, data, left, 0);
		if (rc <= 0) {
			return -1;
		}
		else {
			data += rc;
			left -= rc;
		}
	}
	*num = ntohl(ret);
	return 0;
}

/************************************************************************
 * SerializableBundle
 ************************************************************************/

namespace SnakeSockets {
	std::ostream& operator<<(std::ostream &strm, const SerializableBundle &a) {
		strm << *a.snake << "\n";
		strm << *a.all_bodies << "\n";
		strm << a.max_size << "\n";
		strm << a.lost << "\n";
		strm << a.won << "\n";
		strm << a.ate << "\n";

		return strm;
	}

	void SerializableBundle::rebuildFromString(std::string &serialized) {
		std::istringstream strm(serialized);
		strm >> *this->snake;
		strm >> *this->all_bodies;
		strm >> this->max_size;
		strm >> this->lost;
		strm >> this->won;
		strm >> this->ate;
	}
}

/************************************************************************
 * SnakeServer
 ************************************************************************/

namespace SnakeSockets {
	SnakeServer::SnakeServer(double snake_speed, int max_food, int max_score, int max_x, int max_y, int max_clients) {
		this->snake_speed = snake_speed;
		this->max_food = max_food;
		this->max_score = max_score;
		this->max_x = max_x;
		this->max_y = max_y;
		this->max_clients = max_clients;

		this->started = false;
		this->ended = false;

		this->food = new Food(this->max_x, this->max_y, 0);

		this->base_bundle.all_bodies = new BodyList();
		this->base_bundle.snake = new BodyList();
		this->base_bundle.max_size = Vector2D(max_x, max_y);
	}

	SnakeServer::~SnakeServer() {
		delete this->food;
		delete this->base_bundle.all_bodies;
		delete this->base_bundle.snake;

		this->forceShutdown();

		fsync(this->socket_fd);
		closeSocket(this->socket_fd);
	}

	void SnakeServer::killClient(ClientInfo *client) {
			if (!client) {
				return;
			}

			client->running = false;
			delete client->snake;
			delete client->physics;
			delete client->kbd_server;
			if (client->client_thread.joinable()) {
				client->client_thread.join();
			}
			delete client;
	}

	void SnakeServer::forceShutdown() {
		for (ClientInfo *client: this->clients) {
			this->killClient(client);
		}
	}

	int SnakeServer::getConnectedClientsNumber() {
		int number_of_clients = 0;

		for (ClientInfo *client: this->clients) {
			if (client->running) {
				number_of_clients++;
			}
		}

		return number_of_clients;
	}

	bool SnakeServer::init(std::string ip) {
		this->client_size = (socklen_t)sizeof(this->client);
		this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
		this->myself.sin_family = AF_INET;
		this->myself.sin_port = htons(KEYBOARD_PORT);
		if (ip.empty()) {
			this->myself.sin_addr.s_addr = htonl(INADDR_ANY);
		} else {
			inet_aton(ip.c_str(), &(this->myself.sin_addr));
		} 

		{ // Begin gambiarra
			using namespace std;
			if (::bind(this->socket_fd, (struct sockaddr*)&this->myself, sizeof(this->myself)) != 0) {
				std::cerr << "Error binding: " << std::strerror(errno) << std::endl;
				return false;
			}
		} // End gambiarra

		// Listens for connections
		listen(this->socket_fd, 1);
		std::cout << "\nWaiting for client connections!\n" << std::endl;

		// Waits for clients to connect
		while (this->clients.size() < (unsigned long) this->max_clients) {
			int connection_fd;
			connection_fd = accept(socket_fd, (struct sockaddr*)&this->client, &this->client_size);

			// Creates client info object
			ClientInfo *client = new ClientInfo();
			client->connection_fd = connection_fd;
			client->snake = new Snake(Vector2D(this->max_x/2 + 2*this->clients.size(), this->max_y/2),
					Vector2D(0,-this->snake_speed), 5, clients.size() + 1);
			client->physics = new Physics(client->snake, food, this->max_food, this->max_x, this->max_y);
			client->kbd_server = new KeyboardServer();
			client->kbd_server->init(connection_fd);
			client->running = true;
			client->update_now = false;
			client->send_now = false;

			// Launches the client thread
			client->client_thread = std::thread(&SnakeServer::updateClient, this, client);

			// Appends client to list
			this->clients.push_back(client);

			std::cout << "\nNew client connected!\n" << "Max: " << this->max_clients << " Current: " << this->clients.size() << std::endl;
		}

		this->started = true;

		return true;
	}

	void SnakeServer::update(float deltaT) {
		this->deltaT = deltaT;

		// Updates every client
		for (ClientInfo *client: this->clients) {
			if (client->running) {
				client->update_now = true;
			}
		}

		// Waits for every client to update its snake
		for (ClientInfo *client: this->clients) {
			if (client->running) {
				while (client->update_now);
			}
		}

		// Check for collisons between clients
		std::vector<ClientInfo *> to_collide;
		for (ClientInfo *client: this->clients) {
			for (ClientInfo *other: this->clients) {
				if (client == other) {
					continue;
				}
				for (Body *other_body: other->snake->getBodies()) {
					if (other_body->getPosition() == client->snake->getHeadPosition()) {
						to_collide.push_back(client);
						break;
					}
				}
			}
		}

		// Shrinks all collided snakes:
		for (ClientInfo *client: to_collide) {
			client->physics->collide();
		}

		// Builds base bundle
		this->base_bundle.all_bodies->clear();
		this->base_bundle.all_bodies->append(*this->food);
		for (ClientInfo *client: this->clients) {
			if (client->running) {
				this->base_bundle.all_bodies->append(*client->snake);
			}
		}

		// Checks if someone won because others lost
		int clients_lost = 0;
		for (ClientInfo *client: this->clients) {
			if (client->physics->didLose() || !client->running) {
				clients_lost++;
			}
		}
		if (clients_lost >= this->max_clients-1) {
			for (ClientInfo *client: this->clients) {
				if (!client->physics->didLose() && client->running) {
					client->physics->setWin();
				}
			}
			this->ended = true;
		} else {
			// Checks if someone won
			for (ClientInfo *client: this->clients) {
				if (client->physics->didWin()) {
					this->ended = true;
				}
			}
		}

		// Sends bodies to clients
		for (ClientInfo *client: this->clients) {
			if (client->running) {
				client->send_now = true;
			}
		}

		// Waits for every client to send its bundle
		for (ClientInfo *client: this->clients) {
			if (client->running) {
				while (client->update_now);
			}
		}
	}

	void SnakeServer::updateClient(ClientInfo *client) {
		while (client->running) {
			while (!client->update_now && client->running) {
				char c = client->kbd_server->getchar();
				if (this->started) {
					if (c=='w') {
						client->physics->goUp();
					} else if (c=='a') {
						client->physics->goLeft();
					} else if (c=='s') {
						client->physics->goDown();
					} else if (c=='d') {
						client->physics->goRight();
					} else if (c=='q') {
						client->running = false;
					}
				}
			}

			if (!client->kbd_server->isAlive()) {
				client->running = false;
				client->update_now = false;
			}
			client->physics->update(this->deltaT);
			client->update_now = false;

			while (!client->send_now && client->running);

			SerializableBundle bundle = this->base_bundle;
			bundle.ate = client->physics->didEat();
			bundle.won = client->physics->didWin();
			if (this->ended && !client->physics->didWin()) {
				bundle.lost = true;
			} else {
				bundle.lost = client->physics->didLose();
			}
			bundle.snake = client->snake;

			std::ostringstream strm;
			strm << bundle;

			if (send_int(client->connection_fd, strm.str().length()) == -1) {
				client->running = false;
				client->send_now = false;
			} else if (send(client->connection_fd, strm.str().c_str(), strm.str().length(), 0) == -1) {
				client->running = false;
				client->send_now = false;
			}

			client->send_now = false;
		}
		std::cout << "Closing socket" << std::endl;
		closeSocket(client->connection_fd);
	}

	bool SnakeServer::didEnd() {
		return this->ended;
	}
}

/************************************************************************
 * SnakeClient
 ************************************************************************/

namespace SnakeSockets {
	SnakeClient::SnakeClient() {
		this->bundle.all_bodies = new BodyList();
		this->bundle.snake = new BodyList();
		this->got_first_package = false;
		this->did_update = false;
	}

	SnakeClient::~SnakeClient() {
		this->kbd_client.stop();
		closeSocket(this->socket_fd);
		if (this->client_thread.joinable()) {
			this->client_thread.join();
		}
		delete this->bundle.all_bodies;
		delete this->bundle.snake;
	}

	bool SnakeClient::init(std::string ip) {
		// Initializing server socket
		this->socket_fd = socket(AF_INET, SOCK_STREAM, 0);

		// Trying to connect to server
		this->target.sin_family = AF_INET;
		this->target.sin_port = htons(KEYBOARD_PORT);
		inet_aton(ip.c_str(), &(target.sin_addr));
		if (connect(this->socket_fd, (struct sockaddr*)&this->target, sizeof(target)) != 0) {
			return false;
		}

		// Launches keyboard thread
		this->kbd_client.init(this->socket_fd);

		// Waits for keyboard to come online:
		while (!this->kbd_client.isAlive());

		// Launches client update thread
		this->client_thread = std::thread(&SnakeClient::updateBundle, this);

		return true;
	}

	void SnakeClient::updateBundle() {
		while (this->isAlive()){
			int message_size;

			if (receive_int(this->socket_fd, &message_size) == -1) {
				this->kbd_client.stop();
				break;
			}

			char *message_buff = new char[message_size+1];
			char *message_pointer = message_buff;
			int bytes_left = message_size;

			while (bytes_left > 0 && this->isAlive()) {
				int last_read_size = recv(this->socket_fd, message_pointer, bytes_left, 0);
				if (last_read_size <= 0) {
					this->kbd_client.stop();
				} else {
					message_pointer += last_read_size;
					bytes_left -= last_read_size;
				}
			}

			// Makes sure message is null terminated
			message_buff[message_size] = '\0';

			if (this->isAlive()) {
				// Converts buffer to string
				std::string message_string(message_buff);

				// Critical session, locks to prevent bundle being read 
				// while it is being updated
				this->bundle_lock.lock();
				bool ate = false;
				if (this->bundle.ate) {
					ate = true;
				}
				this->bundle.rebuildFromString(message_string);
				if (!this->bundle.ate) {
					this->bundle.ate = ate;
				}
				if (this->bundle.lost || this->bundle.won) {
					this->kbd_client.stop();
				}
				this->did_update = true;
				this->bundle_lock.unlock();
			}

			this->got_first_package = true;
			delete[] message_buff;
		}
	}

	void SnakeClient::updateBodiesAndTarget(BodyList *bl, BodyList *target) {
		while (!this->got_first_package && this->isAlive());

		this->bundle_lock.lock();
		if (this->did_update) {
			bl->clear();
			bl->append(*this->bundle.all_bodies);
			target->clear();
			target->append(*this->bundle.snake); 
			this->did_update = false;
		}
		this->bundle_lock.unlock();
	}

	bool SnakeClient::isAlive() {
		return this->kbd_client.isAlive();
	}

	bool SnakeClient::didEat() {
		this->bundle_lock.lock();
		bool ate = this->bundle.ate;
		this->bundle.ate = false;
		this->bundle_lock.unlock();
		return ate;
	}

	bool SnakeClient::didLose() {
		this->bundle_lock.lock();
		bool lost = this->bundle.lost;
		this->bundle_lock.unlock();
		return lost;
	}

	bool SnakeClient::didWin() {
		this->bundle_lock.lock();
		bool won = this->bundle.won;
		this->bundle_lock.unlock();
		return won;
	}

	int SnakeClient::getMaxX() {
		return this->bundle.max_size.x;
	}

	int SnakeClient::getMaxY() {
		return this->bundle.max_size.y;
	}
}

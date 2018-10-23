// Autor: Marco Antonio Steck Filho - RA:183374

#include <cstring>
#include <unistd.h>
#include <string>
#include <sstream>
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
    do {
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
    while (left > 0);
    return 0;
}

static int receive_int(int fd, int *num) {
    int32_t ret;
    char *data = (char*)&ret;
    int left = sizeof(ret);
    int rc;
    do {
        rc = recv(fd, data, left, 0);
        if (rc <= 0) {
            if (errno != EINTR) {
                return -1;
            }
        }
        else {
            data += rc;
            left -= rc;
        }
    }
    while (left > 0);
    *num = ntohl(ret);
    return 0;
}
/************************************************************************
 * SerializableBundle
 ************************************************************************/
namespace SnakeSockets {
	std::ostream& operator<<(std::ostream &strm, const SerializableBundle &a) {
		strm << a.snake << "\n";
		strm << a.all_bodies << "\n";
		strm << a.lost << "\n";
		strm << a.won << "\n";
		strm << a.ate << "\n";

		return strm;
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

		this->food = new Food(this->max_x, this->max_y, 0);
	}

	bool SnakeServer::init() {
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

		// Waits for clients to connect
		while (this->clients.size() < (unsigned long) this->max_clients) {
			int connection_fd;
			connection_fd = accept(socket_fd, (struct sockaddr*)&this->client, &this->client_size);

			// Creates client info object
			ClientInfo *client = new ClientInfo();
			client->connection_fd = connection_fd;
			client->snake = new Snake(Vector2D(this->max_x/2 + 2*this->clients.size(), this->max_y/2),
				   	Vector2D(0,this->snake_speed), 5, clients.size() + 1);
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
		}

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

		// TODO: Check for collisons between clients

		// Builds base bundle
		this->base_bundle.all_bodies->clear();
		this->base_bundle.all_bodies->append(*this->food);
		for (ClientInfo *client: this->clients) {
			if (client->running) {
				this->base_bundle.all_bodies->append(*client->snake);
			}
		}

		// Checks if someone won
		for (ClientInfo *client: this->clients) {
			if (client->physics->didWin()) {
				this->ended = true;
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
			while (!client->update_now) {
				char c = client->kbd_server->getchar();
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

			if (!client->kbd_server->isAlive()) {
				client->running = false;
				closeSocket(client->connection_fd);
			}
			client->physics->update(this->deltaT);
			client->update_now = false;

			while (!client->send_now);

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
				closeSocket(client->connection_fd);
			} else if (send(client->connection_fd, strm.str().c_str(), strm.str().length(), 0) == -1) {
				client->running = false;
				client->send_now = false;
				closeSocket(client->connection_fd);
			}
			
			client->send_now = false;
		}
	}
}

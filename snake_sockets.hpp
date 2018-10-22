// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef SNAKE_SERVER_HPP
#define SNAKE_SERVER_HPP

#include <mutex>
#include <vector>

#include "body.hpp"
#include "food.hpp"
#include "keyboard.hpp"
#include "physics.hpp"
#include "snake.hpp"

namespace SnakeSockets {
	struct ClientInfo {
		int connection_fd;
		Snake *snake;
		Physics *motor;
		KeyboardServer *kbd_server;
	};

	class SerializableBundle {
		Snake *snake;
		BodyList *all_bodies;

		void rebuildFromString(std::string &str);

		bool lost;
		bool won;
		bool ate;

		friend std::ostream& operator<<(std::ostream &strm, const SerializableBundle &a);
	};

	class SnakeServer {
		private:
			// Common food object
			Food *food;
			
			// Motor informations
			int max_food;
			int max_score;
			double snake_speed;
			
			// Server information
			bool started;
			bool ended;

			// Field size
			int max_x;
			int max_y;
			
			// Clients
			int max_clients;
			std::vector<ClientInfo *> clients;

			// Threading
			std::mutex mutex;

			// Socket stuff
			int socket_fd;
			socklen_t client_size;
			struct sockaddr_in myself, client;

		public:
			SnakeServer(double snake_speed, int max_food, int max_score, int max_x, int max_y, int max_clients);
			~SnakeServer();

			bool init();
			void update(float deltaT);

			bool didEnd();
	};

	class SnakeClient {

	};
}

#endif //PHYSICS_HPP

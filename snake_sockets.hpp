// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef SNAKE_SERVER_HPP
#define SNAKE_SERVER_HPP

#include <vector>
#include <mutex>

#include "body.hpp"
#include "food.hpp"
#include "keyboard.hpp"
#include "physics.hpp"
#include "snake.hpp"

namespace SnakeSockets {
	struct ClientInfo {
		int connection_fd;
		bool running;
		bool update_now;
		bool send_now;
		Snake *snake;
		Physics *physics;
		KeyboardServer *kbd_server;
		std::thread client_thread;
	};

	class SerializableBundle {
		public:
			BodyList *snake;
			BodyList *all_bodies;

			void rebuildFromString(std::string &str);

			Vector2D max_size;

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
			SerializableBundle base_bundle;
			std::vector<ClientInfo *> clients;

			// Threading
			void updateClient(ClientInfo *client);
			float deltaT;

			// Socket stuff
			int socket_fd;
			socklen_t client_size;
			struct sockaddr_in myself, client;

			// Internal stuff no one gives a sh*t about
			void killClient(ClientInfo *client);

		public:
			SnakeServer(double snake_speed, int max_food, int max_score, int max_x, int max_y, int max_clients);
			~SnakeServer();

			bool init(std::string ip="");
			void update(float deltaT);

			bool didEnd();
			int getConnectedClientsNumber();

			void forceShutdown();
	};

	class SnakeClient {
		private:
			// Threading
			std::mutex bundle_lock;
			KeyboardClient kbd_client;
			std::thread client_thread;
			void updateBundle();
			bool did_update;

			// Socket stuff
			int socket_fd;
			struct sockaddr_in target;
			bool got_first_package;
			
			SerializableBundle bundle;
		
		public:
			SnakeClient();
			~SnakeClient();

			bool init(std::string ip);

			void updateBodiesAndTarget(BodyList *bl, BodyList *taget);

			bool isAlive();
			bool didEat();
			bool didLose();
			bool didWin();

			int getMaxX();
			int getMaxY();
	};
}

#endif //PHYSICS_HPP

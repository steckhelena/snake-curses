// Autor: Marco Antonio Steck Filho - RA:183374

#ifndef BODY_HPP
#define BODY_HPP

#include <vector>
#include <string>

#include "utils.hpp"

using namespace std;

class BodyFrame {
	private:
		int pivot_x;
		int pivot_y;

		unsigned char color;

		vector<string> frame;

	public:
		BodyFrame(int pivot_x, int pivot_y, vector<string> frame);
		BodyFrame(int pivot_x, int pivot_y, string frame);
		BodyFrame(const BodyFrame &bf);
		int getPivotX();
		int getPivotY();
		vector<string> getFrame();
};

class BodyFrames {
	private:
		BodyFrame *up;
		BodyFrame *down;
		BodyFrame *left;
		BodyFrame *right;

	public:
		BodyFrames(BodyFrame *up, BodyFrame *down,  BodyFrame *left, BodyFrame *right);
		BodyFrames(BodyFrame *any);
		BodyFrame *getUp();
		BodyFrame *getDown();
		BodyFrame *getLeft();
		BodyFrame *getRight();
};

class Body {
	private:
		Vector2D speed;
		Vector2D position;
		BodyFrames *frames;

	public:
		Body(Vector2D speed, Vector2D position, BodyFrames *frames);
		Vector2D getPosition();
		void setPosition(Vector2D position);
		Vector2D getSpeed();
		void setSpeed(Vector2D speed);
		BodyFrames *getFrames();
		BodyFrame *getCurrentFrame();
};

class BodyList {
	private:
		vector<Body*> *bodies;

	public:
		BodyList();
		void hardCopy(BodyList *ldc);
		void addBody(Body *c);
		void removeBody(Body *c);
		vector<Body*> *getBodies();
};

#endif //BODY_HPP

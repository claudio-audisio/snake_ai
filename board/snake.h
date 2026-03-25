#pragma once
#include <deque>
#include <raylib.h>
#include <vector>

#include "common/constants.h"
#include "common/utils.h"

using namespace std;


class Snake
{
public:
	int direction;
	deque<Vector2> snake;

	Snake() {}

	void init() {
		snake.clear();
		snake.push_back({randomXPos(), randomYPos()});
		direction = randomInt(0, 3);
	}

	void drawHead(Vector2 pos) {
		DrawRectangleRounded({pos.x + 5, pos.y + 5, CELL_SIZE - 10, CELL_SIZE - 10}, 0.5, 1, LIGHTGRAY);

		switch (direction) {
			case UP:
			DrawCircle(pos.x + 15, pos.y + 15, 5, BG_COLOR);
			DrawCircle(pos.x + CELL_SIZE - 15, pos.y + 15, 5, BG_COLOR);
			break;
			case RIGHT:
			DrawCircle(pos.x + CELL_SIZE - 15, pos.y + 15, 5, BG_COLOR);
			DrawCircle(pos.x + CELL_SIZE - 15, pos.y + CELL_SIZE - 15, 5, BG_COLOR);
			break;
			case DOWN:
			DrawCircle(pos.x + 15, pos.y + CELL_SIZE - 15, 5, BG_COLOR);
			DrawCircle(pos.x + CELL_SIZE - 15, pos.y + CELL_SIZE - 15, 5, BG_COLOR);
			break;
			case LEFT:
			DrawCircle(pos.x + 15, pos.y + 15, 5, BG_COLOR);
			DrawCircle(pos.x + 15, pos.y + CELL_SIZE - 15, 5, BG_COLOR);
			break;
		}
	}

	void drawBody(Vector2 pos) {
		DrawRectangleRounded({pos.x + 5, pos.y + 5, CELL_SIZE - 10, CELL_SIZE - 10}, 0.5, 1, LIGHTGRAY);
	}

	void draw() {
		drawHead(snake.at(0));

		for(int i = 1; i < snake.size(); i++) {
			drawBody(snake.at(i));
		}
	}

	Vector2 getHead() const {
		return snake.at(0);
	}

	Vector2 getNearHead(const int direction) const {
		Vector2 head = snake.at(0);

		switch (direction) {
			case UP: head.y -= CELL_SIZE; break;
			case RIGHT: head.x += CELL_SIZE; break;
			case DOWN: head.y += CELL_SIZE; break;
			case LEFT: head.x -= CELL_SIZE; break;
			default: break;
		}

		return head;
	}

	void move(const Vector2 newHead, const int direction, const bool eat) {
		this->direction = direction;
		snake.push_front(newHead);

		if (!eat) {
			snake.pop_back();
		}
	}

	Vector2 getNewHead(const int direction) const {
		const Vector2 head = snake.at(0);
		Vector2 newHead = {head.x, head.y};

		switch (direction) {
			case UP:  newHead.y -= CELL_SIZE; break;
			case RIGHT: newHead.x += CELL_SIZE; break;
			case DOWN: newHead.y += CELL_SIZE; break;
			case LEFT: newHead.x -= CELL_SIZE; break;
			default: break;
		}

		return newHead;
	}

	bool isBody(const Vector2 pos) const {
		for (const Vector2 body : snake) {
			if (posEquals(body, pos)) {
				return true;
			}
		}

		return false;
	}

	int isOppositeDirection(const int direction) const {
		return this->direction != direction && direction % 2 == this->direction % 2;
	}

};

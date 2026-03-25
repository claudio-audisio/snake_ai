#pragma once

#include <raylib.h>

#include "snake.h"
#include "common/constants.h"
#include "common/utils.h"

using namespace std;


class Board {
public:
	Texture2D strawberry;
	Vector2 sbPos;
	Snake snake;

    Board() {}

	void init() {
    	strawberry = LoadTexture("../asset/strawberry_50.png");
    	reset();
    }

	void reset() {
    	snake.init();
    	newStrawberry();
    }

    void drawBoard() {
		for (int i = 0; i < WINDOW_WIDTH; i += CELL_SIZE) {
			DrawLineEx({static_cast<float>(i), 0}, {static_cast<float>(i), WINDOW_HEIGHT}, 1, LINE_COLOR);
		}

    	for (int i = CELL_SIZE; i < WINDOW_HEIGHT; i += CELL_SIZE) {
    		DrawLineEx({0, static_cast<float>(i)}, {WINDOW_WIDTH, static_cast<float>(i)}, 1, LINE_COLOR);
    	}

    	snake.draw();
    	drawStrawberry();
    }

    void newStrawberry() {
	    Vector2 newPos = {randomXPos(), randomYPos()};

    	while (snake.isBody(newPos)) {
    		newPos = {randomXPos(), randomYPos()};
    	}

		sbPos = newPos;
    }

	void drawStrawberry() const {
    	DrawTextureV(strawberry, sbPos, WHITE);
    }

	float moveSnake(const int direction) {
    	const Vector2 newHead = snake.getNewHead(direction);

    	if (isHitting(newHead, direction)) {
    		snake.move(newHead, direction, false);
    		return DEATH;
    	}

    	const bool eat = posEquals(newHead, sbPos);
    	snake.move(newHead, direction, eat);

    	if (eat) {
    		newStrawberry();
    		return EAT;
    	}

    	return SURVIVED;
    }

	static bool isWall(const Vector2 newHead) {
    	return newHead.x < 0 || newHead.x >= WINDOW_WIDTH || newHead.y < 0 || newHead.y >= WINDOW_HEIGHT;
    }

	bool isHitting(const Vector2 pos, const int direction) const {
	    return snake.isBody(pos) || isWall(pos) || snake.isOppositeDirection(direction);
    }

	tiny_dnn::vec_t* getState() const {
	    auto state = new tiny_dnn::vec_t(12, 0);

    	(*state)[snake.direction] = 1;
    	(*state)[UP + 4] = snake.getHead().y > sbPos.y;
    	(*state)[RIGHT + 4] = snake.getHead().x < sbPos.x;
    	(*state)[DOWN + 4] = snake.getHead().y < sbPos.y;
    	(*state)[LEFT + 4] = snake.getHead().x > sbPos.x;
    	(*state)[UP + 8] = isHitting(snake.getNearHead(UP), UP);
    	(*state)[RIGHT + 8] = isHitting(snake.getNearHead(RIGHT), RIGHT);
    	(*state)[DOWN + 8] = isHitting(snake.getNearHead(DOWN), DOWN);
    	(*state)[LEFT + 8] = isHitting(snake.getNearHead(LEFT), LEFT);

    	return state;
    }

};


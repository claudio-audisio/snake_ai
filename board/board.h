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
	Snake snakeG;
	Snake snakeY;

    Board() {}

	void init() {
    	strawberry = LoadTexture("../asset/strawberry_50.png");
    	reset();
    }

	void reset() {
    	snakeG.init(DARKGREEN);
    	snakeY.init(GOLD);

    	while (posEquals(snakeY.getHead(), snakeG.getHead())) {
    		snakeY.init(GOLD);
    	}

    	newStrawberry();
    }

    void drawBoard() {
		for (int i = 0; i < WINDOW_WIDTH; i += CELL_SIZE) {
			DrawLineEx({static_cast<float>(i), 0}, {static_cast<float>(i), WINDOW_HEIGHT}, 1, LINE_COLOR);
		}

    	for (int i = CELL_SIZE; i < WINDOW_HEIGHT; i += CELL_SIZE) {
    		DrawLineEx({0, static_cast<float>(i)}, {WINDOW_WIDTH, static_cast<float>(i)}, 1, LINE_COLOR);
    	}

    	snakeG.draw();
    	snakeY.draw();
    	drawStrawberry();
    }

    void newStrawberry() {
	    Vector2 newPos = {randomXPos(), randomYPos()};

    	while (snakeG.isBody(newPos) || snakeY.isBody(newPos)) {
    		newPos = {randomXPos(), randomYPos()};
    	}

		sbPos = newPos;
    }

	void drawStrawberry() const {
    	DrawTextureV(strawberry, sbPos, WHITE);
    }

	pair<float, float> moveSnakes(const int gDirection, const int yDirection) {
    	float gReward = SURVIVED, yReward = SURVIVED;
    	const Vector2 newGHead = snakeG.getNewHead(gDirection);
    	const Vector2 newYHead = snakeY.getNewHead(yDirection);

    	if (isGHitting(newGHead, gDirection)) {
    		gReward = DEATH;
    	}

    	if (isYHitting(newYHead, yDirection)) {
    		yReward = DEATH;
    	}

    	if (posEquals(newGHead, newYHead)) {
    		gReward = DEATH;
    		yReward = DEATH;
    	}

    	const bool gEat = gReward != DEATH && posEquals(newGHead, sbPos);
    	snakeG.move(newGHead, gDirection, gEat);

    	const bool yEat = yReward != DEATH && posEquals(newYHead, sbPos);
    	snakeY.move(newYHead, yDirection, yEat);

    	if (gEat || yEat) {
    		newStrawberry();
    	}

    	gReward = gEat ? EAT : gReward;
    	yReward = yEat ? EAT : yReward;

    	return {gReward, yReward};
    }

	static bool isWall(const Vector2 newHead) {
    	return newHead.x < 0 || newHead.x >= WINDOW_WIDTH || newHead.y < 0 || newHead.y >= WINDOW_HEIGHT;
    }

	bool isGHitting(const Vector2 pos, const int direction) const {
	    return snakeG.isBody(pos) || snakeY.isBody(pos) || isWall(pos) || snakeG.isOppositeDirection(direction);
    }

	bool isYHitting(const Vector2 pos, const int direction) const {
    	return snakeG.isBody(pos) || snakeY.isBody(pos) || isWall(pos) || snakeY.isOppositeDirection(direction);
    }

	tiny_dnn::vec_t* getGState() const {
	    auto state = new tiny_dnn::vec_t(12, 0);

    	(*state)[snakeG.direction] = 1;
    	(*state)[UP + 4] = snakeG.getHead().y > sbPos.y;
    	(*state)[RIGHT + 4] = snakeG.getHead().x < sbPos.x;
    	(*state)[DOWN + 4] = snakeG.getHead().y < sbPos.y;
    	(*state)[LEFT + 4] = snakeG.getHead().x > sbPos.x;
    	(*state)[UP + 8] = isGHitting(snakeG.getNearHead(UP), UP);
    	(*state)[RIGHT + 8] = isGHitting(snakeG.getNearHead(RIGHT), RIGHT);
    	(*state)[DOWN + 8] = isGHitting(snakeG.getNearHead(DOWN), DOWN);
    	(*state)[LEFT + 8] = isGHitting(snakeG.getNearHead(LEFT), LEFT);

    	return state;
    }

	tiny_dnn::vec_t* getYState() const {
    	auto state = new tiny_dnn::vec_t(12, 0);

    	(*state)[snakeY.direction] = 1;
    	(*state)[UP + 4] = snakeY.getHead().y > sbPos.y;
    	(*state)[RIGHT + 4] = snakeY.getHead().x < sbPos.x;
    	(*state)[DOWN + 4] = snakeY.getHead().y < sbPos.y;
    	(*state)[LEFT + 4] = snakeY.getHead().x > sbPos.x;
    	(*state)[UP + 8] = isYHitting(snakeY.getNearHead(UP), UP);
    	(*state)[RIGHT + 8] = isYHitting(snakeY.getNearHead(RIGHT), RIGHT);
    	(*state)[DOWN + 8] = isYHitting(snakeY.getNearHead(DOWN), DOWN);
    	(*state)[LEFT + 8] = isYHitting(snakeY.getNearHead(LEFT), LEFT);

    	return state;
    }

};


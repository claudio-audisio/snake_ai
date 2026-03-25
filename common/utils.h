#pragma once

#include <random>
#include <raylib.h>

#include "constants.h"

using namespace std;


int randomInt(const int min, const int max) {
	static mt19937 rng(random_device{}());
	uniform_int_distribution<int> dist(min, max);
	return dist(rng);
}

float randomXPos() {
	return randomInt(0, WINDOW_WIDTH / CELL_SIZE - 1) * CELL_SIZE;
}

float randomYPos() {
	return randomInt(0, WINDOW_HEIGHT / CELL_SIZE - 1) * CELL_SIZE;
}

bool posEquals(const Vector2 pos1, const Vector2 pos2) {
	return pos1.x == pos2.x && pos1.y == pos2.y;
}
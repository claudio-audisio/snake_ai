#pragma once

#include <raylib.h>
#include "cl_random.h"

#include "constants.h"

using namespace std;


float randomXPos() {
	return cl::randomInt(0, WINDOW_WIDTH / CELL_SIZE - 1) * CELL_SIZE;
}

float randomYPos() {
	return cl::randomInt(0, WINDOW_HEIGHT / CELL_SIZE - 1) * CELL_SIZE;
}

bool posEquals(const Vector2 pos1, const Vector2 pos2) {
	return pos1.x == pos2.x && pos1.y == pos2.y;
}
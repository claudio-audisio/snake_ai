#define CNN_USE_OMP

#include "agent/agent.h"
#include "board/board.h"
#include "common/constants.h"

Board board;
Agent agent;
double epsilon = 1;

void init() {
	//SetConfigFlags(FLAG_WINDOW_UNDECORATED);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "snake");
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
	SetTargetFPS(FPS);
	board.init();
	agent.init();
}

int newDirection(tiny_dnn::vec_t* state) {
	if (drand48() < epsilon) {
		return randomInt(0, 3);
	}

	return agent.getAction(state);
}

int main() {
	init();

	unsigned long long i = 0;
	int score = 0;
	int games = 0;
	int step = 0;

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BG_COLOR);

		board.drawBoard();

		EndDrawing();

		tiny_dnn::vec_t state = board.getState();
		const int direction = newDirection(&state);
		const float reward = board.moveSnake(direction);
		tiny_dnn::vec_t nextState = board.getState();
		agent.save(&state, direction, reward, &nextState, reward == DEATH);
		step++;

		if (reward == DEATH) {
			games++;
			board.reset();
			cout << "game " << games << " - memory " << agent.getMemorySize() << " - step " << step << " - score " << score << endl;
			epsilon = max(0.01, epsilon * 0.995);
			i = score = step = 0;
		} else if (reward == EAT) {
			score++;
		}

		agent.train(TRAINING_SIZE);
	}

	CloseWindow();

	return 0;
}
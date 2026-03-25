#define CNN_USE_OMP

#include "agent/agent.h"
#include "board/board.h"
#include "common/constants.h"

Board board;
Agent agent(TRAINING_SIZE);
double epsilon = 1;
int games = 0, step = 0, score = 0;
deque<int> steps;
deque<int> scores;


void init() {
	//SetConfigFlags(FLAG_WINDOW_UNDECORATED);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "snake");
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
	SetTargetFPS(FPS);
	board.init();
	agent.init();
}

int newDirection(const tiny_dnn::vec_t* state) {
	if (drand48() < epsilon) {
		return randomInt(0, 3);
	}

	return agent.getAction(state);
}

void addStep(const int value) {
	steps.push_back(value);

	if (steps.size() > 100) {
		steps.pop_front();
	}
}

void addScore(const int value) {
	scores.push_back(value);

	if (scores.size() > 100) {
		scores.pop_front();
	}
}

float mean(const deque<int>& values) {
	int sum = 0;

	for (const int value : values) {
		sum += value;
	}

	return static_cast<float>(sum) / values.size();
}

void stepFwd(const bool update = true) {
	tiny_dnn::vec_t* state = board.getState();
	const int direction = newDirection(state);
	const float reward = board.moveSnake(direction);
	tiny_dnn::vec_t* nextState = board.getState();
	agent.save(state, direction, reward, nextState, reward == DEATH);
	step++;

	if (reward == DEATH) {
		games++;
		board.reset();
		steps.push_back(step);
		scores.push_back(score);
		score = step = 0;
		epsilon = max(0.01, epsilon * 0.995);

		cout << "game " << games << " - memory " << agent.getMemorySize() << " - step " << mean(steps) << " - score " << mean(scores) << endl;

		if (update) {
			agent.updateNet();
		}
	} else if (reward == EAT) {
		score++;
	}
}

void initMemory() {
	while (agent.getMemorySize() < TRAINING_SIZE) {
		stepFwd(false);
	}
}

int main() {
	init();
	initMemory();

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BG_COLOR);

		board.drawBoard();

		EndDrawing();

		stepFwd();
	}

	CloseWindow();

	return 0;
}
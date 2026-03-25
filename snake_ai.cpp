#define CNN_USE_OMP

#include "agent/agent.h"
#include "board/board.h"
#include "common/constants.h"

Board board;
Agent agentG(TRAINING_SIZE);
Agent agentY(TRAINING_SIZE);
double epsilon = 1;
int games = 0, step = 0, gScore = 0, yScore = 0;
deque<int> steps;
deque<int> gScores;
deque<int> yScores;


void init() {
	//SetConfigFlags(FLAG_WINDOW_UNDECORATED);
	InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "snake");
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN);
	SetTargetFPS(FPS);
	board.init();
	agentG.init();
	agentY.init();
}

int newDirectionG(const tiny_dnn::vec_t* state) {
	if (drand48() < epsilon) {
		return randomInt(0, 3);
	}

	return agentG.getAction(state);
}

int newDirectionY(const tiny_dnn::vec_t* state) {
	if (drand48() < epsilon) {
		return randomInt(0, 3);
	}

	return agentY.getAction(state);
}

void addStep(const int value) {
	steps.push_back(value);

	if (steps.size() > 100) {
		steps.pop_front();
	}
}

void addScore(deque<int>& values, const int value) {
	values.push_back(value);

	if (values.size() > 100) {
		values.pop_front();
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
	tiny_dnn::vec_t* gState = board.getGState();
	const int gDirection = newDirectionG(gState);
	tiny_dnn::vec_t* yState = board.getYState();
	const int yDirection = newDirectionG(yState);
	const pair<float, float> rewards = board.moveSnakes(gDirection, yDirection);
	tiny_dnn::vec_t* nextGState = board.getGState();
	agentG.save(gState, gDirection, rewards.first, nextGState, rewards.first == DEATH);
	tiny_dnn::vec_t* nextYState = board.getYState();
	agentY.save(yState, yDirection, rewards.second, nextYState, rewards.second == DEATH);
	step++;

	if (rewards.first == DEATH || rewards.second == DEATH) {
		games++;
		board.reset();
		steps.push_back(step);
		addScore(gScores, gScore);
		addScore(yScores, yScore);
		gScore = yScore = step = 0;
		epsilon = max(0.01, epsilon * 0.995);

		cout << "game " << games << " - memory " << agentG.getMemorySize() << "/" << agentY.getMemorySize()
			<< " - step " << mean(steps) << " - score " << mean(gScores) << "/" << mean(yScores) << endl;

		if (update) {
			agentG.updateNet();
			agentY.updateNet();
		}
	} else if (rewards.first == EAT) {
		gScore++;
	} else if (rewards.second == EAT) {
		yScore++;
	}
}

void initMemory() {
	while (agentG.getMemorySize() < TRAINING_SIZE && agentY.getMemorySize() < TRAINING_SIZE) {
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
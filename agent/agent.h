#pragma once
#include <atomic>
#include <mutex>
#include <thread>

#include "tiny_dnn/tiny_dnn.h"
#include "memory.h"

using namespace std;

class Agent {
public:
	tiny_dnn::network<tiny_dnn::sequential> onLineNet;
	tiny_dnn::network<tiny_dnn::sequential> trainingNet;
	tiny_dnn::adam optimizer;
	float gamma = 0.9;
	Memory memory;
	mutex tNetMtx;
	atomic<bool> readyToTrain{false}, running{true};
	thread trainingThread;
	int lastUpdate = 0, generation = 0;

	Agent(const int startUpdating) {
		lastUpdate = startUpdating - UPDATE_FREQ;
	}

	~Agent() {
		running = false;
		trainingThread.join();
	}

	void init() {
		onLineNet << tiny_dnn::layers::fc(12, 256)  << tiny_dnn::activation::relu()
			<< tiny_dnn::layers::fc(256, 128) << tiny_dnn::activation::relu()
			<< tiny_dnn::layers::fc(128, 4);

		trainingNet << tiny_dnn::layers::fc(12, 256)  << tiny_dnn::activation::relu()
			<< tiny_dnn::layers::fc(256, 128) << tiny_dnn::activation::relu()
			<< tiny_dnn::layers::fc(128, 4);

		optimizer.alpha = 0.001;

		trainingThread = thread([this]() {
			runner(TRAINING_SIZE);
		});
	}

	void updateNet() {
		if (memory.size() > lastUpdate + UPDATE_FREQ) {
			cout << "generation " << ++generation << endl;
			lastUpdate += UPDATE_FREQ;
			lock_guard<mutex> lock(tNetMtx);

			for (int i = 0; i < trainingNet.depth(); i++) {
				vector<tiny_dnn::vec_t*> fromWeights = trainingNet[i]->weights();
				vector<tiny_dnn::vec_t*> toWeights   = onLineNet[i]->weights();

				for (int j = 0; j < fromWeights.size(); j++) {
					*toWeights[j] = *fromWeights[j];
				}
			}
		}
	}

	int getAction(const tiny_dnn::vec_t* state) {
		tiny_dnn::vec_t q = onLineNet.predict(*state);
		int best = 0;

		for (int i = 1; i < 4; i++) {
			if (q[i] > q[best]) best = i;
		}

		return best;
	}

	void save(tiny_dnn::vec_t* state, const int direction, const float reward, tiny_dnn::vec_t* vector, const bool gameOver) {
		memory.push(state, direction, reward, vector, gameOver);
		readyToTrain = true;
	}

	void train(const int trainingSize) {
		if (memory.size() < trainingSize) return;

		//cout << "training..." << endl;
		// random sample from memory
		const vector<Experience*> samples = memory.getSamples(trainingSize);

		// build input and target tensors
		vector<tiny_dnn::vec_t> inputs;
		vector<tiny_dnn::vec_t> targets;

		lock_guard<mutex> lock(tNetMtx);

		for (auto sample : samples) {
			tiny_dnn::vec_t target = trainingNet.predict(*sample->state);

			if (sample->gameOver) {
				target[sample->direction] = sample->reward;
			} else {
				tiny_dnn::vec_t next_q = trainingNet.predict(*sample->nextState);
				const float best = *max_element(next_q.begin(), next_q.end());
				target[sample->direction] = sample->reward + gamma * best;
			}

			inputs.push_back(*sample->state);
			targets.push_back(target);
		}

		// train — tiny-dnn handles backpropagation internally
		trainingNet.fit<tiny_dnn::mse>(optimizer, inputs, targets, 1, 1);
	}

	int getMemorySize() const {
		return memory.size();
	}

	void runner(const int trainingSize) {
		while (running) {
			if (readyToTrain) {
				readyToTrain = false;
				train(trainingSize);
			} else {
				this_thread::sleep_for(chrono::milliseconds(100));
			}
		}
	}

};

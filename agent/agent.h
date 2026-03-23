#pragma once
#include "tiny_dnn/tiny_dnn.h"
#include "memory.h"

using namespace std;

class Agent {
public:
	tiny_dnn::network<tiny_dnn::sequential> net;
	tiny_dnn::adam optimizer;
	float gamma = 0.9;
	Memory memory;

	Agent() = default;

	void init() {
		net << tiny_dnn::layers::fc(8, 256)  << tiny_dnn::activation::relu()
			<< tiny_dnn::layers::fc(256, 128) << tiny_dnn::activation::relu()
			<< tiny_dnn::layers::fc(128, 4);

		optimizer.alpha = 0.001;
	}

	tiny_dnn::vec_t predict(tiny_dnn::vec_t* state) {
		return net.predict(*state);
	}

	int getAction(tiny_dnn::vec_t* state) {
		tiny_dnn::vec_t q = predict(state);
		int best = 0;

		for (int i = 1; i < 4; i++) {
			if (q[i] > q[best]) best = i;
		}

		return best;
	}

	void save(tiny_dnn::vec_t* state, const int direction, const float reward, tiny_dnn::vec_t* vector, const bool gameOver) {
		memory.push(state, direction, reward, vector, gameOver);
	}

	void train(const int trainingSize) {
		if (memory.size() < trainingSize) return;

		//cout << "training..." << endl;
		// random sample from memory
		const vector<Experience*> samples = memory.getSamples(trainingSize);

		// build input and target tensors
		vector<tiny_dnn::vec_t> inputs;
		vector<tiny_dnn::vec_t> targets;

		for (auto sample : samples) {
			tiny_dnn::vec_t target = predict(sample->state);

			if (sample->gameOver) {
				target[sample->direction] = sample->reward;
			} else {
				tiny_dnn::vec_t next_q = predict(sample->nextState);
				const float best = *max_element(next_q.begin(), next_q.end());
				target[sample->direction] = sample->reward + gamma * best;
			}

			inputs.push_back(*sample->state);
			targets.push_back(target);
		}

		// train — tiny-dnn handles backpropagation internally
		net.fit<tiny_dnn::mse>(optimizer, inputs, targets, 1, 1);
	}

	int getMemorySize() const {
		return memory.size();
	}
};

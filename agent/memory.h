#pragma once
#include <deque>

#include "common/constants.h"
#include "common/type.h"

using namespace std;

class Memory {
public:
	deque<Experience> memory;

	Memory() = default;

	void push(tiny_dnn::vec_t* state, const int direction, const float reward, tiny_dnn::vec_t* nextState, const bool gameOver) {
		memory.push_back({state, direction, reward, nextState, gameOver});

		if (memory.size() > MAX_MEMORY) {
			memory.pop_front();
		}
	}

	int size() const {
		return memory.size();
	}
	
	vector<Experience*> getSamples(const int amount) {
		vector<Experience*> samples;

		mt19937 rng{random_device{}()};
		uniform_int_distribution<int> dist(0, memory.size() - 1);
    
		for (int i = 0; i < amount; i++) {
			const int idx = dist(rng);
			samples.push_back(&memory[idx]);
		}
    
		return samples;
	}

};
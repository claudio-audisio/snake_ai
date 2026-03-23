#pragma once
#include <vector>

using namespace std;


struct Experience {
	tiny_dnn::vec_t*	state;
	int					direction;
	float				reward;
	tiny_dnn::vec_t*	nextState;
	bool				gameOver;
};

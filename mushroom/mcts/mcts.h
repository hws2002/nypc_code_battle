// mcts.h
#pragma once
#include "utils.h"
#include <vector>
#include <memory>
#include <random>

using namespace std;

//전방선언
class MCTSNode;

Move runMCTS(shared_ptr<class MCTSNode> rootNode, 
			 const std::vector<std::vector<int>>& board, 
			 int myTime, 
			 bool myTurn);

// rollout (시뮬레이션)
bool simulate(shared_ptr<class MCTSNode> selected, string method="random");

// 현재 상태에서 가능한 모든 유효한 직사각형 Move를 생성
// vector<Move> getAllValidMoves(const vector<vector<int>>& board);
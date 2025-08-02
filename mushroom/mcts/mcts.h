// mcts.h
#pragma once

#include "game.h"
#include <vector>
#include <memory>
#include <random>


Move runMCTS(const std::vector<std::vector<int>>& board, int iterations, bool myTurn);

// rollout (시뮬레이션)
bool simulate(const vector<vector<int>>& initBoard, bool myStartTurn, string method);

bool isValid(Board board, int r1, int c1, int r2, int c2);

// 현재 상태에서 가능한 모든 유효한 직사각형 Move를 생성
vector<Move> getAllValidMoves(const vector<vector<int>>& board);


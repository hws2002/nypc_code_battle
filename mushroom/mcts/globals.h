#pragma once

#include <vector>
#include <random>
#include <unordered_map>

using namespace std;

const int MAX_ROWS = 110;
const int MAX_COLS = 17;
const int MAX_VAL = 3;

extern uint64_t zobristBoard[MAX_ROWS][MAX_COLS][MAX_VAL]; // board[r][c] == v 일 때
extern uint64_t zobristMyScore[171]; // myScore 해싱 (0~999 범위)
extern uint64_t zobristOppScore[171]; // oppScore 해싱
extern uint64_t zobristTurn[2]; // myTurn 해싱

void initZobrist(); //초기화함수

uint64_t computeZobristHash(const vector<vector<int>> & board, int myScore, int oppScore, bool myTurn) ;

#pragma once

#include <vector>
#include <random>
#include <memory>
#include <unordered_map>

//전방선언
class MCTSNode;

using namespace std;

constexpr int MAX_ROWS = 10;
constexpr int MAX_COLS = 17;
// 한 칸이 가질 수 있는 상태 개수 (예: 내말(0), 상대(-1), 땅 1..9 => 총 11)
constexpr int MAX_VAL = 11;

// 점수 범위 (0..170 포함)
constexpr int MAX_SCORE_PLUS1 = 171; // 인덱스 0..170 사용


extern uint64_t zobristBoard[MAX_ROWS][MAX_COLS][MAX_VAL]; // board[r][c] == v 일 때
extern uint64_t zobristMyScore[171]; // myScore 해싱 (0~999 범위)
extern uint64_t zobristOppScore[171]; // oppScore 해싱
extern uint64_t zobristTurn[2]; // myTurn 해싱
extern unordered_map<uint64_t, shared_ptr<MCTSNode>> transpositionTable;

void initZobrist(); //초기화함수

// 전체 보드 + score + turn으로 해시 계산 (완전 계산)
uint64_t computeZobristHash(const vector<vector<int>> & board, int myScore, int oppScore, bool myTurn);

// 증분 업데이트 도움 함수들 (O(1))
inline void xorCellHash(uint64_t &hash, int r, int c, int cellValue);
inline void xorMyScoreHash(uint64_t &hash, int myScore);
inline void xorOppScoreHash(uint64_t &hash, int oppScore);
inline void xorTurnHash(uint64_t &hash, bool myTurn);

// 유틸 : transposition table 초기화 / 크기 예약
inline void ttClear();
inline void ttReserve(size_t n);
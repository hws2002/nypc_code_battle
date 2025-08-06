#include "globals.h"

uint64_t zobristBoard[MAX_ROWS][MAX_COLS][MAX_VAL];
uint64_t zobristMyScore[171];
uint64_t zobristOppScore[171];
uint64_t zobristTurn[2];

void initZobrist() {
    mt19937_64 rng(20250806); // 고정된 시드 (디버깅에 유리)
    uniform_int_distribution<uint64_t> dist;

    for (int r = 0; r < MAX_ROWS; ++r)
        for (int c = 0; c < MAX_COLS; ++c)
            for (int v = 0; v < MAX_VAL; ++v)
                zobristBoard[r][c][v] = dist(rng);

    for (int i = 0; i < 171; ++i) {
        zobristMyScore[i] = dist(rng);
        zobristOppScore[i] = dist(rng);
    }

    zobristTurn[0] = dist(rng);
    zobristTurn[1] = dist(rng);
}

uint64_t computeZobristHash(const vector<vector<int>> &board, int myScore, int oppScore, bool myTurn) {
    uint64_t h = 0;

    for (int r = 0; r < board.size(); ++r)
        for (int c = 0; c < board[r].size(); ++c) {
            int v = board[r][c];
            if (v >= 0 && v < MAX_VAL) {
                h ^= zobristBoard[r][c][v];
            }
        }

    if (myScore >= 0 && myScore < 171)
        h ^= zobristMyScore[myScore];
    if (oppScore >= 0 && oppScore < 171)
        h ^= zobristOppScore[oppScore];

    h ^= zobristTurn[myTurn ? 1 : 0];

    return h;
}
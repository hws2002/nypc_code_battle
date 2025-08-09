#include "globals.h"

uint64_t zobristBoard[MAX_ROWS][MAX_COLS][MAX_VAL];
uint64_t zobristMyScore[171];
uint64_t zobristOppScore[171];
uint64_t zobristTurn[2];

unordered_map<uint64_t, shared_ptr<MCTSNode>> transpositionTable;

void initZobrist() {
    mt19937_64 rng(20250809); // 고정된 시드 (디버깅에 유리)
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
	        // map value -> index in 0..MAX_VAL-1
            // 여기서 보드값이 음수일 수 있으므로, 사용 중인 인덱스 매핑 규칙을 통일해야 함.
            // 예: 내말=0 -> index 0, 상대=-1 -> index 1, 땅 1..9 -> indices 2..10
			int idx;
			if (v == 0) idx = 0;
			else if (v == -1) idx = 1;
			else idx = 1 + v;
            if (v >= 0 && v < MAX_VAL) {
                h ^= zobristBoard[r][c][idx];
            }
        }

    if (myScore >= 0 && myScore < 171)
        h ^= zobristMyScore[myScore];
    if (oppScore >= 0 && oppScore < 171)
        h ^= zobristOppScore[oppScore];

    h ^= zobristTurn[myTurn ? 1 : 0];

    return h;
}

/* 인라인 헬퍼들: 증분 XOR 처리 */
inline void xorCellHash(uint64_t &hash, int r, int c, int cellValue) {
    int idx;
    if (cellValue == 0) idx = 0;
    else if (cellValue == -1) idx = 1;
    else idx = 1 + cellValue;
    hash ^= zobristBoard[r][c][idx];
}

inline void xorMyScoreHash(uint64_t &hash, int myScore) {
    if (myScore >= 0 && myScore < MAX_SCORE_PLUS1) hash ^= zobristMyScore[myScore];
}
inline void xorOppScoreHash(uint64_t &hash, int oppScore) {
    if (oppScore >= 0 && oppScore < MAX_SCORE_PLUS1) hash ^= zobristOppScore[oppScore];
}
inline void xorTurnHash(uint64_t &hash, bool myTurn) {
    hash ^= zobristTurn[myTurn ? 1 : 0];
}

inline void ttClear() {
    transpositionTable.clear();
}
inline void ttReserve(size_t n) {
    transpositionTable.reserve(n);
}

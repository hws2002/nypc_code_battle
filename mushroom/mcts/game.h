// game.h
#pragma once

#include "globals.h"
#include "utils.h"
#include <memory>

using namespace std;
using Board = vector<vector<int>>;


//전방선언
class MCTSNode;

class Game{
public:
    Board board; // 게임 보드 (2차원 벡터)
    bool first;                // 선공 여부
    bool passed;               // 마지막 턴에 패스했는지 여부
	int myscore;
	int oppscore;
	list<Move> allValidMoves;
	shared_ptr<class MCTSNode> rootNode;

    Game() {};

    Game(const Board &board, bool first);


    vector<int> calculateMove(int myTime, int oppTime);

	void updateRootNode(const Move & move);
    // 상대방의 수를 받아 보드에 반영
    void updateOpponentAction(const vector<int> &action, int time)
    {
        updateMove(action[0], action[1], action[2], action[3], false);
    }

    // 주어진 수를 보드에 반영 (칸을 0으로 지움)
    void updateMove(int r1, int c1, int r2, int c2, bool isMyMove)
    {
        if (r1 == -1 && c1 == -1 && r2 == -1 && c2 == -1)
        {
            passed = true;
            return;
        }
        for (int r = r1; r <= r2; r++)
            for (int c = c1; c <= c2; c++){
				if( board[r][c] > 0) oppscore++;
                board[r][c] = 0;
			}
        passed = false;
    }
};


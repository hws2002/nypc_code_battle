// game.cpp
#include "game.h"
#include "mcts.h"

#define DEBUG

bool Move::isPass() const {
    return r1 == -1;
}

void Move::printMove() const { //디버그용 함수
	std::cout<<"move : "<<r1<<" "<<c1<<" "<<r2<<" "<<c2<<". size : "<<(r2-r1+1)*(c2-c1+1)<<std::endl;
}

Game::Game(const Board &board, bool first)
	: board(board), first(first), passed(false), myscore(0), oppscore(0) {
		allValidMoves = getAllValidMoves(board);
		#ifdef DEBUG
		for(const auto & vm : allValidMoves){
			vm.printMove();
		}
		#endif
};

vector<int> Game::calculateMove(int myTime, int oppTime){
	//몬테카를로 서치
	cout<<"mytime :"<<myTime<<endl;
	Move best = runMCTS(board, 500, true);
	return {best.r1, best.c1, best.r2, best.c2}; // 유효한 사각형이 없으면 패스
};

// utils

// prefixSum[r][c] 는 (0,0)부터 (r-1,c-1)까지 영역 합
vector<vector<int>> computePrefixSum(const vector<vector<int>>& board) {
    int R = board.size();
    int C = board[0].size();
    vector<vector<int>> prefixSum(R + 1, vector<int>(C + 1, 0));

    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            prefixSum[r][c] = board[r-1][c-1] 
                              + prefixSum[r-1][c] 
                              + prefixSum[r][c-1] 
                              - prefixSum[r-1][c-1];
        }
    }
    return prefixSum;
}

int getRectSum(const vector<vector<int>>& prefixSum, int r1, int c1, int r2, int c2) {
    // r1,c1,r2,c2는 0-based 인덱스
    // prefixSum은 1-based 인덱스
    return prefixSum[r2+1][c2+1] 
           - prefixSum[r1][c2+1] 
           - prefixSum[r2+1][c1] 
           + prefixSum[r1][c1];
}


bool checkBorder(const vector<vector<int>>& board, int r1, int c1, int r2, int c2) {
    bool r1fit = false, r2fit = false, c1fit = false, c2fit = false;
    for (int r = r1; r <= r2; ++r) {
        if (board[r][c1] > 0) c1fit = true;
        if (board[r][c2] > 0) c2fit = true;
    }
    for (int c = c1; c <= c2; ++c) {
        if (board[r1][c] > 0) r1fit = true;
        if (board[r2][c] > 0) r2fit = true;
    }
    return r1fit && r2fit && c1fit && c2fit;
}

// game 인스턴스가 초기화될시에, 모든 valid moves를 저장한다.
// mctsnode를 생성할 때마다 validmoves를 탐색하면 비용이 너무 크기 때문
vector<Move> getAllValidMoves(const vector<vector<int>>& board) {
	vector<Move> moves;
	int R = board.size();
	int C = board[0].size();
	const auto & prefixSum = computePrefixSum(board);
    
	for (int r1 = 0; r1 < R; ++r1) {
        for (int c1 = 0; c1 < C; ++c1) {
            for (int r2 = r1; r2 < R; ++r2) {
                for (int c2 = c1; c2 < C; ++c2) {
                    int sum = getRectSum(prefixSum, r1, c1, r2, c2);
                    if (sum == 10 && checkBorder(board, r1, c1, r2, c2)) {
                        moves.push_back({r1, c1, r2, c2});
                    }
                }
            }
        }
    }
	return moves;
}

// 사각형 (r1, c1) ~ (r2, c2)이 유효한지 검사 (합이 10이고, 네 변을 모두 포함)
bool isValid(Board board, int r1, int c1, int r2, int c2){
	int sums = 0;
	bool r1fit = false, c1fit = false, r2fit = false, c2fit = false;
	for (int r = r1; r <= r2; r++)
		for (int c = c1; c <= c2; c++)
			if (board[r][c] != 0)
			{
				sums += board[r][c];
				
				if(sums > 10) return false;
				if (r == r1)
					r1fit = true;
				if (r == r2)
					r2fit = true;
				if (c == c1)
					c1fit = true;
				if (c == c2)
					c2fit = true;
			}
	return (sums == 10) && r1fit && r2fit && c1fit && c2fit;
}

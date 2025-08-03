// game.cpp
#include "utils.h"
#include "game.h"
#include "mctsnode.h"
#include "mcts.h"
#include <fstream>
#include <chrono>

#define DEBUG

Game::Game(const Board &board, bool first)
	: board(board), first(first), passed(false), myscore(0), oppscore(0) {
// auto start = std::chrono::high_resolution_clock::now();
		allValidMoves = getAllValidMoves(board);

		rootNode = make_shared<MCTSNode>(board, first, Move(-1,-1,-1,-1),
										allValidMoves, nullptr);
		
// auto end = std::chrono::high_resolution_clock::now();
// auto duration = std::chrono::duration_cast<std::chrono::microseconds >(end-start);

// std::cout << "실행 시간: " << duration.count() << "microseconds" << std::endl;
		#ifdef DEBUG
			cout<<"numnber of valid moves : "<<rootNode->validmoves.size()<<endl;
			std::ofstream fout("initial_valid3.txt");
			for (const auto &vm : allValidMoves) {
				fout << "move : " << vm.r1 << " " << vm.c1 << " " << vm.r2 << " " << vm.c2
					 << ". size : " << vm.size << std::endl;
			}
			fout.close();

		#endif
}

vector<int> Game::calculateMove(int myTime, int oppTime){
	Move best = runMCTS(this->rootNode, board, myTime, true);
	updateRootNode(best);
	return {best.r1, best.c1, best.r2, best.c2}; // 유효한 사각형이 없으면 패스
}

void Game::updateRootNode(const Move & best){
	
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


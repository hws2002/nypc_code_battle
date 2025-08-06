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
		Fenwick2D fenwick = computeFenwickSum(board);
		allValidMoves = getAllValidMoves(board,fenwick);
		unordered_set<Move, MoveHasher> moveSet;
		for(auto & m : allValidMoves){
			moveSet.insert(m);
		}
		rootNode = make_shared<MCTSNode>(board, fenwick, first, Move(-1,-1,-1,-1),
										allValidMoves, moveSet, nullptr,0,0);
		rootNode->validmovesupdated = true;
		
// auto end = std::chrono::high_resolution_clock::now();
// auto duration = std::chrono::duration_cast<std::chrono::microseconds >(end-start);

// std::cout << "실행 시간: " << duration.count() << "microseconds" << std::endl;
		#ifdef DEBUG
			cout<<"number of valid moves : "<<rootNode->validmoves.size()<<endl;
			std::ofstream fout("initial_valid1.txt");
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
	// for(auto & child : rootNode->children){
	// 	if(child->move == best){
	// 		// 새 루트 노드로 이동
	// 		rootNode = child;
	// 		rootNode->parent = nullptr;
	// 	}
	// }
}


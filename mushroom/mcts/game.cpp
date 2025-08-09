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

		// 초기 zobrist 해시 계산
    	uint64_t rootHash = computeZobristHash(board, 0, 0, first);

    	// 트랜스포지션 테이블에 등록
    	transpositionTable[rootHash] = rootNode;
		rootNode->hashKey = rootHash;
		
// auto end = std::chrono::high_resolution_clock::now();
// auto duration = std::chrono::duration_cast<std::chrono::microseconds >(end-start);

// std::cout << "실행 시간: " << duration.count() << "microseconds" << std::endl;
		#ifdef DEBUG
			cout<<"number of valid moves : "<<rootNode->validmoves.size()<<endl;
			std::ofstream fout("initial_valid.txt");
			for (const auto &vm : allValidMoves) {
				fout << "move : " << vm.r1 << " " << vm.c1 << " " << vm.r2 << " " << vm.c2
					 << ". size : " << vm.size << std::endl;
			}
			fout.close();
		#endif
}

vector<int> Game::calculateMove(int myTime, int oppTime){
	cout<<"hello"<<endl;
	Move best = runMCTS(this->rootNode, board, myTime, true);
	updateRootNode(best, true);
	return {best.r1, best.c1, best.r2, best.c2}; // 유효한 사각형이 없으면 패스
}

void Game::updateOpponentAction(const Move & action, int time){
	updateRootNode(action, false);
}

void Game::updateRootNode(const Move & best, bool myTurn){
	// 자식 노드 중 best와 같은 수가 있는지 탐색
	for(auto &child : rootNode->children){
		if( child->move == best){
			rootNode = child;
			rootNode->parents.clear();
			if(!rootNode->validmovesupdated){
				updateValidMoves(rootNode->board,
								rootNode->fenwick,
								best,
								rootNode->validmoves,
								rootNode->moveSet);
			}
			rootNode->validmovesupdated = true;
			return;
		}
	}

	// children에 없을 경우, 새로 생성해야 함 
	// (이 경우가 자주 생기지 않기를 바래야 함)
	auto newBoard = rootNode->board;
	auto newFw = rootNode->fenwick;
	int ms = rootNode->myScore;
	int os = rootNode->oppScore;
	bool turn = !myTurn;

	// move를 보드와 점수에 반영
	if(best.r1 != -1){
		for(int r = best.r1; r <= best.r2; r++){
			for(int c = best.c1; c <= best.c2; c++){
				if(myTurn){ //best가 내 무브
					if(newBoard[r][c] == -1){ //상대땅
						os--;
						ms++;
					} else if (newBoard[r][c] == 0) { // 내땅

					} else { // 비어있는 땅
						ms++;
					}
					newBoard[r][c] = 0;
				} else { //best가 상대방 무브
					if(newBoard[r][c] == -1){ //상대땅

					} else if (newBoard[r][c] == 0) { // 내땅
						ms--;
						os++;
					} else { // 비어있는 땅
						os++;
					}
				}
			}
		}
	}

	uint64_t newHash = computeZobristHash(newBoard, ms, os, turn);

	auto it = transpositionTable.find(newHash);
	if( it != transpositionTable.end()){
		rootNode = it->second;
		rootNode->parents.clear();
		if(!rootNode->validmovesupdated){
				updateValidMoves(rootNode->board,
								rootNode->fenwick,
								best,
								rootNode->validmoves,
								rootNode->moveSet);
		}
		rootNode->validmovesupdated = true;
		return ;
	}
	
	
	// 만약 이미 생성한적도 없다면, 새로 생성한다
	auto vML = rootNode->validmoves;
	auto mS = rootNode->moveSet;
	rootNode = make_shared<MCTSNode>(newBoard, newFw, turn, 
									best, vML, mS, nullptr,
									ms, os);
	if(!rootNode->validmovesupdated){
		updateValidMoves(rootNode->board,
			rootNode->fenwick,
			best,
			rootNode->validmoves,
			rootNode->moveSet);
		rootNode->validmovesupdated = true;
	}
	
	rootNode->hashKey = newHash;
	transpositionTable[newHash] = rootNode;
}
// mcts.cpp
#include "mcts.h"
#include "mctsnode.h"
#include "game.h"
#include "utils.h"

// #define DEBUG

// MCTS 수행 함수
Move runMCTS(NodePtr rootNode, 
			 const vector<vector<int>>& board, 
			 int myTime, 
			 bool myTurn) {
	#ifdef DEBUG 
		cout<<"runMCTS"<<endl;
	#endif
	auto start = std::chrono::high_resolution_clock::now();
    srand(time(NULL));
	auto now = std::chrono::high_resolution_clock::now();
	NodePtr node;
	int i = 0;
	while(std::chrono::duration_cast<std::chrono::milliseconds>(now-start).count() < 600 ){ // 600ms 동안 생각
		node = rootNode;
		// Selection
		while (node->isFullyExpanded() && !node->children.empty()) {
			node = *std::max_element(
				node->children.begin(), node->children.end(),
				[](const NodePtr& a, const NodePtr& b) {
				return a->uctValue() < b->uctValue();
			});
		}
		if (!node->validmovesupdated) {
			updateValidMoves(node->board, node->fenwick, node->move, node->validmoves);
			node->validmovesupdated = true;
		}
		#ifdef DEBUG 
			cout<<"expand"<<endl;
		#endif
		// Expansion
		// cout<<"start expansion"<<endl;
		if (!node->isFullyExpanded()) node->expand();
		// cout<<"expansion done"<<endl;
		
		// Simulation
		NodePtr selected = node->children.empty() ? 
			make_shared<MCTSNode>(node->board, node->fenwick, !node->myTurn, Move{-1, -1, -1, -1}, 
								  node->validmoves, node, node->myScore, node->oppScore) : 
			node->children[0];
		#ifdef DEBUG 
			cout<<"simulate"<<endl;
		#endif
		bool result = simulate(selected, "random");
		
		#ifdef DEBUG 
			cout<<"backprop"<<endl;
		#endif
		// Backpropagation
		while (selected) {
			selected->visits++;
			if (selected->myTurn == myTurn && result)
				selected->wins += 1.0;
			else if (selected->myTurn != myTurn && !result)
				selected->wins += 1.0;
			selected = selected->parent.lock(); //weak_ptr -> shared_ptr
		}
		now = std::chrono::high_resolution_clock::now();
		i++;
	}
	cout<<"iterated "<<i<<"times. End runMCTS "<<endl;
	if(rootNode->children.empty()) {
		#ifdef DEBUG
			cout<<"pass this time"<<endl;
		#endif
		return Move{-1,-1,-1,-1};
	}
    return rootNode->bestChild()->move;
}


//TODO : simulation 빠르게 하기
bool simulate(NodePtr selected, string method) {
	// 시뮬레이션 전에, move에 기반하여(이 노드를 만들게 한 수) validmoves(부모의 validmoves와 동일)를 수정한다.
	if( !selected->validmovesupdated ) {
		updateValidMoves(selected->board, selected->fenwick, selected->move, selected->validmoves);
		selected->validmovesupdated = true;
	}
	vector<vector<int>> board = selected->board;
	Fenwick2D fenwick = selected->fenwick;
	int myScore = selected->myScore;
	int oppScore = selected->oppScore;
	bool turn = selected->myTurn;
	list<Move> moves = selected->validmoves;
	Move m;
	if(method == "random"){
		int passCount = 0;
		while (passCount < 4) {
			// skip은 고려하지 않음
			if (moves.empty()) {
				m = Move(-1, -1, -1, -1);
			} else {
				m = moves.back();
			}
			
			if (m.isPass()) {
				passCount++;
			} else {
				if( isValid(board, m.r1, m.c1, m.r2, m.c2)){
					for (int r = m.r1; r <= m.r2; ++r)
						for (int c = m.c1; c <= m.c2; ++c) {
							if (board[r][c] > 0) {
								if (turn) myScore+=m.size;
								else oppScore+=m.size;
							}
							board[r][c] = 0;
						}
					passCount = 0;
					//update valid moves
					updateValidMoves(board, fenwick, m, moves);
				} else {
					moves.pop_back();
				}
			}
			turn = !turn;
		}
		
		return myScore > oppScore; // true: 승, false: 패
	} else { //TODO
		return false;
	}
}
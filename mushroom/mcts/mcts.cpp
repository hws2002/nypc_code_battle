// mcts.cpp
#include "mcts.h"
#include "mctsnode.h"
#include "game.h"
#include "utils.h"

#define DEBUG
// #define DEBUG_SIMUL

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

		#ifdef DEBUG
			cout<<"node->"; node->move.printMove(); cout<<endl;
		#endif

		if (!node->validmovesupdated) {
			updateValidMoves(node->board, node->fenwick, 
							 node->move, node->validmoves,
							node->moveSet);
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
								node->validmoves, node->moveSet, node, node->myScore, node->oppScore) : 
			node->children[0];
		#ifdef DEBUG 
			cout<<"simulate with selected node "; selected->move.printMove(); cout<<endl;
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
		#ifdef DEBUG
		if( i>= 10) break;
		#endif
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
	// 시뮬레이션에서는 따로 moveSet을 수정하지 않는다.
	if( !selected->validmovesupdated ) {
		// 이때는 "추가"만 된다. 
		updateValidMoves(selected->board, selected->fenwick,
						 selected->move, selected->validmoves,
						selected->moveSet);
		selected->validmovesupdated = true;
	}

	vector<vector<int>> board = selected->board;
	Fenwick2D fenwick = selected->fenwick;
	int myScore = selected->myScore;
	int oppScore = selected->oppScore;
	bool turn = selected->myTurn;
	list<Move> moves = selected->validmoves;
	auto moveSet = selected->moveSet;
	cout<<"starts with score : myScore = "<<myScore<<", oppScore = "<<oppScore<<endl;
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
					#ifdef DEBUG_SIMUL
					if(turn){
						cout<<"my ";m.printMove(); cout<<endl;
					} else {
						cout<<"opponent ";m.printMove(); cout<<endl;
					}
					#endif
				turn = !turn;
			} else {
				if( isValid(board, m.r1, m.c1, m.r2, m.c2)){
					for (int r = m.r1; r <= m.r2; ++r)
						for (int c = m.c1; c <= m.c2; ++c) {
							if (board[r][c] > 0) {
								if (turn) 
									myScore++;
								else 
									oppScore++;
								fenwick.update(r, c, -board[r][c]);
								board[r][c] = 0;
							} else {
								if (turn) {
									myScore--;
									oppScore++;
								}
								else {
									oppScore--;
									myScore++;
								}
							}
						}
					passCount = 0;
					#ifdef DEBUG_SIMUL
					if(turn){
						cout<<"my ";m.printMove(); cout<<endl;
					} else {
						cout<<"opponent ";m.printMove(); cout<<endl;
					}
					#endif
					//update valid moves
					updateValidMoves(board, fenwick, m, moves, moveSet);
					turn = !turn;
				} else {
					moves.pop_back();
				}
			}
		}
		
		#ifdef DEBUG_SIMUL
			cout<<"simulation done, myScore : "<<myScore<<", oppScore : "<<oppScore<<endl;
			cout<<"I won : "<<(myScore > oppScore)<<endl;
		#endif
		return myScore > oppScore; // true: 승, false: 패
	} else { //TODO
		return false;
	}
}
// mctsnode.cpp
#include "mcts.h"
#include "mctsnode.h"


// #define DEBUG

using namespace std;

// MCTS Node
MCTSNode::MCTSNode(const vector<vector<int>>& board, Fenwick2D& fenwick, bool myTurn, 
				   Move move, const list<Move> validMoves, unordered_set<Move, MoveHasher>& moveSet,
				  NodePtr parent, int myScore, int oppScore)
	: board(board), fenwick(fenwick), myTurn(myTurn), move(move), 
	validmoves(validMoves),moveSet(moveSet), myScore(myScore), oppScore(oppScore) {
		parents.push_back(parent);
		if(move == Move(-1, -1, -1, -1)) {
			validmovesupdated = true;
		} else {
			validmovesupdated =  false;
			for(int r = move.r1; r <= move.r2; ++r) {
				for (int c = move.c1; c <= move.c2; ++c) {
					if (board[r][c] > 0) {
						this->fenwick.update(r, c, -board[r][c]);
					}
				}
			}
			
			// set board to 0 or -1
			for(int r = move.r1; r <= move.r2; ++r)
				for (int c = move.c1; c <= move.c2; ++c){
					if( myTurn) // move는 나의 턴일때의 action
						this->board[r][c] = 0;
					else
						this->board[r][c] = -1;
				}
		}
		it = validmoves.begin();
	}

bool MCTSNode::isFullyExpanded() const {
	// auto _it = it;
	// return ++_it == validmoves.begin();
	return it == validmoves.end();
}

// double MCTSNode::uctValue() const {
// 	if (visits == 0) return numeric_limits<double>::infinity();
// 	double exploitation = wins / visits;
// 	double exploration = EXPLORATION_CONSTANT * sqrt(log(parent->visits + 1) / visits);
// 	return exploitation + exploration;
// }
double MCTSNode::uctValue() const {
	if (visits == 0) return std::numeric_limits<double>::infinity();
	double exploitation = wins / visits;

	auto p = parent.lock();  // weak_ptr → shared_ptr
	int parentVisits = p ? p->visits : 0;  // p가 nullptr일 수 있음 (루트 노드)

	double exploration = EXPLORATION_CONSTANT * sqrt(log(parentVisits + 1) / visits);
	return exploitation + exploration;
}

void MCTSNode::expand() {
	if (validmoves.empty()) validmoves.push_back({-1, -1, -1, -1});
	if(it != validmoves.end()){
		bool added = false;
		while(!added && it != validmoves.end()){
			auto & m = *it;
			if(isValid(board, m.r1, m.c1, m.r2, m.c2)){

				// child의 validmoves는 생성될때 바로 수정되지 않고, selected 됐을때 수정한다.
				int ms = myScore; int os = oppScore;
				for(int r = m.r1; r <= m.r2; r++){
					for(int c = m.c1; c <= m.c2; c++){
						if(myTurn){ // 내턴
							if( board[r][c] == -1){ //상대땅
								os--;
								ms++;
							} else if (board[r][c] == 0){ //내땅
								//아무것도 안함
							} else {
								ms++;
							}
						} else { //상대턴
							if( board[r][c] == -1) { // 상대땅
								// 아무것도 안함
							} else if (board[r][c] == 0){ //내땅
								ms--;
								os++;
							} else {// 소유권없는 땅
								os++;
							}
						}
					}
				}
				auto self = shared_from_this();
				
				//child 노드 생성시에, board와 fenwick를 수정생성한다.
				auto child = make_shared<MCTSNode>(
					board, fenwick, !myTurn, m,
					validmoves, moveSet, self, ms, os
				);
				
				uint64_t childHash = computeZobristHash(child->board, ms, os, !myTurn);
				child->hashKey = childHash;
				
				auto it = transpositionTable.find(childHash);
				if( it == transpositionTable.end()){
					transpositionTable[childHash] = child;
				} else {
					auto existingNode = it->second;
				}
				children.push_back(child);
				existingNode->parents.push_back(shared_from_this());
				added = true;
				it++;
				
				#ifdef DEBUG
					// cout<<"created child with "; m.printMove(); cout<<endl;
				#endif
			} else { //not valid anymore
				auto it_copy = it;
				moveSet.erase(m);
				it = validmoves.erase(it_copy);
				#ifdef DEBUG
					// m.printMove(); cout<<"is invalid"<<endl;
				#endif
			}
		}
	}
}


//TODO : utils로 옮기기. (시뮬레이션시에 필요함)
// 방법1 : incrementalUpdate
// void MCTSNode::updateValidMoves(){
// 	// this->move가 발생한 후, validmoves를 재정의한다.
// 	int maxHeight = 3; int maxWidth = 3;
// 	int boardHeight = board.size();
// 	int boardWidth = board[0].size();
	
// 	int rStart = max(0, move.r1 - maxHeight);
//     int rEnd = min(boardHeight - 1, move.r2 + maxHeight);
//     int cStart = max(0, move.c1 - maxWidth);
//     int cEnd = min(boardWidth - 1, move.c2 + maxWidth);
	
// 	// Fenwick Tree 업데이트
// 	for(int r = move.r1; r <= move.r2; r++){
// 		for(int c = move.c1; c <= move.c2; c++){
// 			if( board[r][c] )
// 				fenwick.update(r,c, -board[r][c]);
// 		}
// 	}
	
// 	// 후보 영역 탐색
// 	for(int r1 = rStart; r1 <= rEnd; r1++){
// 		for(int r2 = r1; r2 <= rEnd; r2++){
// 			for (int c1 = cStart; c1 <= cEnd; c1++) {
//                 for (int c2 = c1; c2 <= cEnd; c2++) {
//                     if (fenwick.query(r1,c1,r2,c2) == 10 && checkBorder(board,r1,c1,r2,c2)) {
//                         // 유효한 후보 영역으로 저장 또는 처리
// 						validmoves.push_back(Move(r1,c1,r2,c2));
//                     }
//                 }
//             }
// 		}
// 	}
	
// 	this->validmovesupdated = true;
// }

NodePtr MCTSNode::bestChild() const {
	return *max_element(children.begin(), children.end(), 
						[](const NodePtr& a, const NodePtr& b) {
		return a->visits < b->visits;
	});
}

// mcts.cpp
#include "mcts.h"
#include "mctsnode.h"
#include "game.h"
#include "utils.h"

#define DEBUG

// MCTS 수행 함수
Move runMCTS(NodePtr rootNode, 
			 const vector<vector<int>>& board, 
			 int myTime, 
			 bool myTurn) {
	auto start = std::chrono::high_resolution_clock::now();
    srand(time(NULL));
	auto now = std::chrono::high_resolution_clock::now();
		// while(std::chrono::duration_cast<std::chrono::milliseconds>(now-start).count() < 1000 ){ // 1000ms 동안 생각
		// MCTSNode* node = rootNode.get();
		// // Selection
		// auto start = std::chrono::high_resolution_clock::now();
		// while (node->isFullyExpanded() && !node->children.empty()) {
		// node = max_element(
		// node->children.begin(), node->children.end(),
		// [](const NodePtr& a, const NodePtr& b) {
		// return a->uctValue() < b->uctValue();
		// })->get();
		// }

		// // Expansion
		// if (!node->isFullyExpanded()) node->expand();
		// // Simulation
		// NodePtr selected = node->children.empty() ? make_shared<MCTSNode>(node->board, !node->myTurn, Move{-1, -1, -1, -1}, node) : node->children[0];
		// //선택되었을때, move에 기반하여(이 노드를 만들게 한 수) validmoves(부모의 validmoves와 동일)를 수정한다.
		// selected->updateValidMoves();
		// bool result = simulate(selected->board, selected->myTurn,"random");

		// // Backpropagation
		// while (selected) {
		// selected->visits++;
		// if (selected->myTurn == myTurn && result)
		// selected->wins += 1.0;
		// else if (selected->myTurn != myTurn && !result)
		// selected->wins += 1.0;
		// selected = selected->parent;
		// }
		// now = std::chrono::high_resolution_clock::now();
		// }
    return rootNode->bestChild()->move;
}



// bool simulate(const vector<vector<int>>& initBoard, bool myStartTurn, string method) {
// 	if(method == "random"){
// 		vector<vector<int>> board = initBoard;
// 		bool turn = myStartTurn;
// 		int passCount = 0;
// 		int myScore = 0, oppScore = 0;
// 		// vector<Move> moves = updateValidMoves(board, move validmoves);
// 		while (passCount < 2) {
// 			vector<Move> moves = getAllValidMoves(board);
// 			Move m;
// 			if (moves.empty()) {
// 				m = {-1, -1, -1, -1};
// 			} else {
// 				m = moves[rand() % moves.size()];
// 			}

// 			if (m.isPass()) {
// 				passCount++;
// 			} else {
// 				for (int r = m.r1; r <= m.r2; ++r)
// 					for (int c = m.c1; c <= m.c2; ++c) {
// 						if (board[r][c] > 0) {
// 							if (turn) myScore++;
// 							else oppScore++;
// 						}
// 						board[r][c] = 0;
// 					}
// 				passCount = 0;
// 			}
// 			turn = !turn;
// 		}
// 		return myScore > oppScore; // true: 승, false: 패
// 	} else { //TODO
// 		return false;
// 	}
// }



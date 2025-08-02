// mctsnode.cpp
#include "mcts.h"
#include "mctsnode.h"

using namespace std;

// MCTS Node
MCTSNode::MCTSNode(const vector<vector<int>>& board, bool myTurn, Move move, MCTSNode* parent, const vector<Move> validMoves)
	: board(board), myTurn(myTurn), move(move), parent(parent), validmoves(validMoves) {};

bool MCTSNode::isFullyExpanded() const {
	return !children.empty();
}

double MCTSNode::uctValue() const {
	if (visits == 0) return numeric_limits<double>::infinity();
	double exploitation = wins / visits;
	double exploration = EXPLORATION_CONSTANT * sqrt(log(parent->visits + 1) / visits);
	return exploitation + exploration;
}

void MCTSNode::expand() {
	if (validmoves.empty()) validmoves.push_back({-1, -1, -1, -1});

	for (const Move& m : validmoves) {
		auto newBoard = board;
		if (!m.isPass()) {
			for (int r = m.r1; r <= m.r2; ++r)
				for (int c = m.c1; c <= m.c2; ++c)
					newBoard[r][c] = 0;
		}
		// child의 validmoves는 생성될때 바로 수정되지 않고, selected 됐을때 수정한다.
		children.push_back(make_shared<MCTSNode>(newBoard, !myTurn, m, validmoves, this));
	}
}

//TODO:
void MCTSNode::updateValidMoves(){
	
};

NodePtr MCTSNode::bestChild() const {
	return *max_element(children.begin(), children.end(), [](const NodePtr& a, const NodePtr& b) {
		return a->visits < b->visits;
	});
}

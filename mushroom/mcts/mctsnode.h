// mctsnode.h
#pragma once

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <vector>
#include <unordered_set>

using namespace std;
const double EXPLORATION_CONSTANT = sqrt(2.0);
class MCTSNode;
using Board = std::vector<std::vector<int>>;
using NodePtr = std::shared_ptr<class MCTSNode>;

// MCTS Node
class MCTSNode {
public:
    vector<vector<int>> board;
    bool myTurn;
    Move move; // 이 노드를 만들게 한 수
    int visits = 0;
    double wins = 0.0;
	int myScore = 0;
	int oppScore = 0;
    vector<NodePtr> children;
	vector<Move> validmoves;
    NodePtr parent;
	
	MCTSNode(){}; // default generator
    MCTSNode(const vector<vector<int>>& board, bool myTurn,
			 Move move, const vector<Move> validMove,
			 MCTSNode* parent = nullptr);

    bool isFullyExpanded() const;

    double uctValue() const ;

    void expand();

    NodePtr bestChild() const;
	
	void updateValidMoves();
};
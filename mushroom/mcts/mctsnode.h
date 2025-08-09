// mctsnode.h
#pragma once

#include "globals.h"
#include "utils.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>
#include <vector>
#include <unordered_set>
#include <list>


using namespace std;
const double EXPLORATION_CONSTANT = sqrt(2.0);
class MCTSNode;
using Board = std::vector<std::vector<int>>;
using NodePtr = std::shared_ptr<class MCTSNode>;

// MCTS Node
class MCTSNode : public std::enable_shared_from_this<MCTSNode> {
public:
    vector<vector<int>> board;
    bool myTurn;
    Move move; // 이 노드를 만들게 한 수
    int visits = 0;
    double wins = 0.0;
	int myScore = 0;
	int oppScore = 0;
	bool validmovesupdated = false;
    vector<NodePtr> children;
	// vector<Move> validmoves;
	list<Move> validmoves;
    vector<weak_ptr<MCTSNode>> parents;
	Fenwick2D fenwick;
	unordered_set<Move, MoveHasher> moveSet;
    list<Move>::iterator it;
	uint64_t hashKey;
	
	MCTSNode(){}; // default generator
    MCTSNode(const vector<vector<int>>& board, Fenwick2D & fenwick, bool myTurn,
			 Move move, const list<Move> validMoves, unordered_set<Move, MoveHasher>& moveSet,
			 NodePtr parent = nullptr, int myScore = 0, int oppScore = 0);
	

    bool isFullyExpanded() const;

    double uctValue() const ;

    void expand();

    NodePtr bestChild() const;
	
	void updateValidMoves();
};
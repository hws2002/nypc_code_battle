// mctsnode.cpp
#include "mctsnode.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <algorithm>

using namespace std;

const double EXPLORATION_CONSTANT = sqrt(2.0);

class MCTSNode;
using Board = std::vector<std::vector<int>>;
using NodePtr = std::shared_ptr<class MCTSNode>;

// 현재 상태에서 가능한 모든 유효한 직사각형 Move를 생성
vector<Move> getAllValidMoves(const vector<vector<int>>& board);
bool isValidRect(const vector<vector<int>>& board, int r1, int c1, int r2, int c2);

bool Move::isPass() const {
    return r1 == -1;
}

// MCTS Node
class MCTSNode {
public:
    vector<vector<int>> board;
    bool myTurn;
    Move move; // 이 노드를 만들게 한 수
    int visits = 0;
    double wins = 0.0;
    vector<NodePtr> children;
    NodePtr parent;

    MCTSNode(const vector<vector<int>>& board, bool myTurn, Move move, MCTSNode* parent = nullptr)
        : board(board), myTurn(myTurn), move(move), parent(parent) {}

    bool isFullyExpanded() const {
        return !children.empty();
    }

    double uctValue() const {
        if (visits == 0) return numeric_limits<double>::infinity();
        double exploitation = wins / visits;
        double exploration = EXPLORATION_CONSTANT * sqrt(log(parent->visits + 1) / visits);
        return exploitation + exploration;
    }

    void expand() {
        vector<Move> validMoves = getAllValidMoves(board);
        if (validMoves.empty()) validMoves.push_back({-1, -1, -1, -1});

        for (const Move& m : validMoves) {
            auto newBoard = board;
            if (!m.isPass()) {
                for (int r = m.r1; r <= m.r2; ++r)
                    for (int c = m.c1; c <= m.c2; ++c)
                        newBoard[r][c] = 0;
            }
            children.push_back(make_shared<MCTSNode>(newBoard, !myTurn, m, this));
        }
    }

    NodePtr bestChild() const {
        return *max_element(children.begin(), children.end(), [](const NodePtr& a, const NodePtr& b) {
            return a->visits < b->visits;
        });
    }
};

// 랜덤 시뮬레이션 (rollout)
bool simulate(const vector<vector<int>>& initBoard, bool myStartTurn, string method) {
	if(method == "random"){
		vector<vector<int>> board = initBoard;
		bool turn = myStartTurn;
		int passCount = 0;
		int myScore = 0, oppScore = 0;

		while (passCount < 2) {
			vector<Move> moves = getAllValidMoves(board);
			Move m;
			if (moves.empty()) {
				m = {-1, -1, -1, -1};
			} else {
				m = moves[rand() % moves.size()];
			}

			if (m.isPass()) {
				passCount++;
			} else {
				for (int r = m.r1; r <= m.r2; ++r)
					for (int c = m.c1; c <= m.c2; ++c) {
						if (board[r][c] > 0) {
							if (turn) myScore++;
							else oppScore++;
						}
						board[r][c] = 0;
					}
				passCount = 0;
			}
			turn = !turn;
		}
		return myScore > oppScore; // true: 승, false: 패
	}
}

// MCTS 수행 함수
Move runMCTS(const vector<vector<int>>& board, int iterations, bool myTurn) {
    srand(time(NULL));
    auto root = make_shared<MCTSNode>(board, myTurn, Move{-1, -1, -1, -1});

    for (int i = 0; i < iterations; ++i) {
        MCTSNode* node = root.get();

        // Selection
        while (node->isFullyExpanded() && !node->children.empty()) {
            node = max_element(
                node->children.begin(), node->children.end(),
                [](const NodePtr& a, const NodePtr& b) {
                    return a->uctValue() < b->uctValue();
                })->get();
        }

        // Expansion
        if (!node->isFullyExpanded()) node->expand();

        // Simulation
        NodePtr selected = node->children.empty() ? make_shared<MCTSNode>(node->board, !node->myTurn, Move{-1, -1, -1, -1}, node) : node->children[0];
        bool result = simulate(selected->board, selected->myTurn,"random");

        // Backpropagation
        while (selected) {
            selected->visits++;
            if (selected->myTurn == myTurn && result)
                selected->wins += 1.0;
            else if (selected->myTurn != myTurn && !result)
                selected->wins += 1.0;
            selected = selected->parent;
        }
    }

    return root->bestChild()->move;
}

// 직사각형 조건 검증
bool isValidRect(const vector<vector<int>>& board, int r1, int c1, int r2, int c2) {
    int sum = 0, count = 0;
    bool r1fit = false, r2fit = false, c1fit = false, c2fit = false;
    for (int r = r1; r <= r2; ++r) {
        for (int c = c1; c <= c2; ++c) {
            if (board[r][c] > 0) {
                sum += board[r][c];
                count++;
                if (r == r1) r1fit = true;
                if (r == r2) r2fit = true;
                if (c == c1) c1fit = true;
                if (c == c2) c2fit = true;
            }
            if (sum > 10) return false;
        }
    }
    return sum == 10 && r1fit && r2fit && c1fit && c2fit;
}

// 가능한 모든 직사각형을 찾기
vector<Move> getAllValidMoves(const vector<vector<int>>& board) {
    vector<Move> moves;
    int R = board.size();
    int C = board[0].size();
    for (int r1 = 0; r1 < R; ++r1) {
        for (int c1 = 0; c1 < C; ++c1) {
            for (int r2 = r1; r2 < R; ++r2) {
                for (int c2 = c1; c2 < C; ++c2) {
                    if (isValidRect(board, r1, c1, r2, c2)) {
                        moves.push_back({r1, c1, r2, c2});
                    }
                }
            }
        }
    }
    return moves;
}

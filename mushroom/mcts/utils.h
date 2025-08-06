#pragma once
#include <chrono>
#include <vector>
#include <iostream>
#include <algorithm>
#include <list>
#include <unordered_set>

using namespace std;

struct Move{
public:
    int r1, c1, r2, c2;
	int size;
    bool isPass() const;
	void printMove() const;
	Move(int r1, int c1, int r2, int c2);
	Move(){}
    bool operator==(const Move& other) const {
        return r1 == other.r1 && c1 == other.c1 &&
               r2 == other.r2 && c2 == other.c2 &&
               size == other.size;
    }
};

struct MoveHasher {
    size_t operator()(const Move& m) const {
        size_t h1 = hash<int>()(m.r1);
        size_t h2 = hash<int>()(m.c1);
        size_t h3 = hash<int>()(m.r2);
        size_t h4 = hash<int>()(m.c2);
        size_t h5 = hash<int>()(m.size);

        // Combine hashes (standard hash combine technique)
        return (((((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1)) >> 1) ^ (h4 << 1)) ^ (h5 << 1);
    }
};

class Fenwick2D {
public:
    int n, m;
    vector< vector<int> > tree;

	Fenwick2D(){};
	
	// 1-based structure
    Fenwick2D(int rows, int cols);

	// 0-based indexing
    void update(int x, int y, int delta);

    int query(int x, int y) const;

    int query(int x1, int y1, int x2, int y2) const; 
};

// prefixSum[r][c] 는 (0,0)부터 (r-1,c-1)까지 영역 합
vector<vector<int>> computePrefixSum(const vector<vector<int>>& board);

// 펜윅트리 기반 valid moves 찾기
Fenwick2D computeFenwickSum(const vector<vector<int>>& board);
							
int getRectSum(const vector<vector<int>>& prefixSum,
			   int r1, int c1, int r2, int c2);

bool checkBorder(const vector<vector<int>>& board, 
				 int r1, int c1, int r2, int c2);

// Fenwick2D or 2D PrefixSum
list<Move> getAllValidMoves(const vector<vector<int>>& board, Fenwick2D& fenwickSum);
// vector<Move> getAllValidMoves(const vector<vector<int>>& board);


//TODO : utils로 옮기기. (시뮬레이션시에 필요함)
// 방법1 : incrementalUpdate
void updateValidMoves(const vector<vector<int>>& board, Fenwick2D& fenwick, 
					  Move& move, list<Move>& validmoves,
					 unordered_set<Move, MoveHasher>& moveSet);

bool isValid(const vector<vector<int>>& board, int r1, int c1, int r2, int c2);

// 조브리스팅




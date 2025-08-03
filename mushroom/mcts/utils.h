#pragma once
#include <chrono>
#include <vector>
#include <iostream>
#include <vector>

using namespace std;

struct Move{
public:
    int r1, c1, r2, c2;
	int size;
    bool isPass() const;
	void printMove() const;
	Move(){};
	Move(int r1, int c1, int r2, int c2);
};

class Fenwick2D {
private:
    int n, m;
    vector<vector<int>> tree;

public:
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
vector<Move> getAllValidMoves(const vector<vector<int>>& board);
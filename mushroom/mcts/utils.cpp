#include "utils.h"

Move::Move(int r1, int c1, int r2, int c2): r1(r1),c1(c1),r2(r2),c2(c2)
{
	if( r1 != -1){
		size = (r2-r1+1)*(c2-c1+1);
	} else size = r1;
}

bool Move::isPass() const {
    return r1 == -1;
}

void Move::printMove() const { //디버그용 함수
	std::cout<<"move : "<<r1<<" "<<c1<<" "<<r2<<" "<<c2<<". size : "<<(r2-r1+1)*(c2-c1+1)<<std::endl;
}

// 2D 펜윅트리 구현
Fenwick2D::Fenwick2D(int rows, int cols) : n(rows), m(cols) {
	tree.assign(n + 1, vector<int>(m + 1, 0));
}

void Fenwick2D::update(int x, int y, int delta) {
	for (int i = x + 1; i <= n; i += i & -i) {
		for (int j = y + 1; j <= m; j += j & -j) {
			tree[i][j] += delta;
		}
	}
}

int Fenwick2D::query(int x, int y) const {
	int sum = 0;
	for (int i = x + 1; i > 0; i -= i & -i) {
		for (int j = y + 1; j > 0; j -= j & -j) {
			sum += tree[i][j];
		}
	}
	return sum;
}

int Fenwick2D::query(int x1, int y1, int x2, int y2) const {
	return query(x2, y2)
		 - query(x1 - 1, y2)
		 - query(x2, y1 - 1)
		 + query(x1 - 1, y1 - 1);
}


// prefixSum[r][c] 는 (0,0)부터 (r-1,c-1)까지 영역 합
vector<vector<int>> computePrefixSum(const vector<vector<int>>& board) {
    int R = board.size();
    int C = board[0].size();
    vector<vector<int>> prefixSum(R + 1, vector<int>(C + 1, 0));

    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            prefixSum[r][c] = board[r-1][c-1] 
                              + prefixSum[r-1][c] 
                              + prefixSum[r][c-1] 
                              - prefixSum[r-1][c-1];
        }
    }
    return prefixSum;
}


// 펜윅트리 기반 valid moves 찾기
Fenwick2D computeFenwickSum(const vector<vector<int>>& board) {
    int R = board.size();
    int C = board[0].size();
	Fenwick2D fenwickSum(R,C);

    for (int r = 0; r < R; ++r) {
        for (int c = 0; c < C; ++c) {
			fenwickSum.update(r,c, board[r][c]);
        }
    }
    return fenwickSum;
}

int getRectSum(const vector<vector<int>>& prefixSum, int r1, int c1, int r2, int c2) {
    // r1,c1,r2,c2는 0-based 인덱스
    // prefixSum은 1-based 인덱스
    return prefixSum[r2+1][c2+1] 
           - prefixSum[r1][c2+1] 
           - prefixSum[r2+1][c1] 
           + prefixSum[r1][c1];
}


bool checkBorder(const vector<vector<int>>& board, int r1, int c1, int r2, int c2) {
    bool r1fit = false, r2fit = false, c1fit = false, c2fit = false;
    for (int r = r1; r <= r2; ++r) {
        if (board[r][c1] > 0) c1fit = true;
        if (board[r][c2] > 0) c2fit = true;
    }
    for (int c = c1; c <= c2; ++c) {
        if (board[r1][c] > 0) r1fit = true;
        if (board[r2][c] > 0) r2fit = true;
    }
    return r1fit && r2fit && c1fit && c2fit;
}

// Fenwick2D 기반
vector<Move> getAllValidMoves(const vector<vector<int>>& board, Fenwick2D& fenwickSum) {
	vector<Move> moves;
	int R = fenwickSum.n;
	int C = fenwickSum.m;
	// const auto & prefixSum = computePrefixSum(board);
    // const auto & fenwickSum = computeFenwickSum(board);
	for (int r1 = 0; r1 < R; ++r1) {
        for (int c1 = 0; c1 < C; ++c1) {
            for (int r2 = r1; r2 < R; ++r2) {
                for (int c2 = c1; c2 < C; ++c2) {
                    int sum = fenwickSum.query(r1, c1, r2, c2);
                    if (sum == 10 && checkBorder(board, r1, c1, r2, c2)) {
                        moves.push_back(Move(r1, c1, r2, c2));
                    }
                }
            }
        }
    }
	
	
	// 땅 크기를 기준으로 오름차순 정렬
	sort(moves.begin(), moves.end(), [](const Move&a, const Move&b){
		return a.size < b.size;
	});
	
	return moves;
}

// // prefixsum기반
// vector<Move> getAllValidMoves(const vector<vector<int>>& board) {
// 	vector<Move> moves;
// 	int R = board.size();
// 	int C = board[0].size();
// 	const auto & prefixSum = computePrefixSum(board);
    
// 	for (int r1 = 0; r1 < R; ++r1) {
//         for (int c1 = 0; c1 < C; ++c1) {
//             for (int r2 = r1; r2 < R; ++r2) {
//                 for (int c2 = c1; c2 < C; ++c2) {
//                     int sum = getRectSum(prefixSum, r1, c1, r2, c2);
//                     if (sum == 10 && checkBorder(board, r1, c1, r2, c2)) {
//                         moves.push_back(Move(r1, c1, r2, c2));
//                     }
//                 }
//             }
//         }
//     }
// 	return moves;
// } 



// 사각형 (r1, c1) ~ (r2, c2)이 유효한지 검사 (합이 10이고, 네 변을 모두 포함)
bool isValid(const vector<vector<int>>& board, int r1, int c1, int r2, int c2){
	int sums = 0;
	bool r1fit = false, c1fit = false, r2fit = false, c2fit = false;
	for (int r = r1; r <= r2; r++)
		for (int c = c1; c <= c2; c++)
			if (board[r][c] != 0)
			{
				sums += board[r][c];
				
				if(sums > 10) return false;
				if (r == r1)
					r1fit = true;
				if (r == r2)
					r2fit = true;
				if (c == c1)
					c1fit = true;
				if (c == c2)
					c2fit = true;
			}
	return (sums == 10) && r1fit && r2fit && c1fit && c2fit;
}
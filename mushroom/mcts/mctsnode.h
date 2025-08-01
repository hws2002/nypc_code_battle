// mctsnode.h
#pragma once

#include <vector>
#include <memory>
#include <random>

struct Move {
    int r1, c1, r2, c2;
    bool isPass() const;
};

Move runMCTS(const std::vector<std::vector<int>>& board, int iterations, bool myTurn);

#ifndef BOARD_HPP
#define BOARD_HPP

#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define TURN 1

#include <vector>
#include <iostream>

typedef struct {
    std::vector<std::vector<int>> board;
    unsigned int flags;
} game;

game makeGame();
void readBoard(std::vector<std::vector<int>> &board, bool ids);

#endif

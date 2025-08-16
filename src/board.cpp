#include "board.hpp"

game makeGame() {
  game Game;

  for (size_t i = 0; i < 8; i++) {  // COLUMN
    std::vector<int> row;
    for (size_t j = 0; j < 8; j++) {  // ROW
      row.push_back(0);
      if (i == 0 || i == 7) {
        if (j == 0 || j == 7) {
          row[j] = ROOK;
        } else if (j == 1 || j == 6) {
          row[j] = KNIGHT;
        } else if (j == 2 || j == 5) {
          row[j] = BISHOP;
        } else if (j == 3) {
          if (i == 0) {
            row[j] = QUEEN;
          } else {
            row[j] = KING;
          }
        } else if (j == 4) {
          if (i == 0) {
            row[j] = KING;
          } else {
            row[j] = QUEEN;
          }
        }
      } else if (i == 1 || i == 6) {
        row[j] = 1;
      }
    }
    Game.board.push_back(row);
  }

  Game.flags = 0;
  return Game;
}

void readBoard(std::vector<std::vector<int>> &board, bool ids) {
  for (size_t i = 0; i < board.size(); i++) {
    for (size_t j = 0; j < board[i].size(); j++) {
      if (ids) {
        std::cout << "[" << board[i][j] << "] ";
      } else {
        std::cout << "[";
        int v = board[i][j];
        char c = 0;
        if (v == 1) {
          c = 'P';
        } else if (v == 2) {
          c = 'k';
        } else if (v == 3) {
          c = 'B';
        } else if (v == 4) {
          c = 'R';
        } else if (v == 5) {
          c = 'Q';
        } else if (v == 6) {
          c = 'K';
        } else if (v == 0) {
          c = ' ';
        }
        std::cout << c << "] ";
      }
    }
    std::cout << std::endl;
  }
}

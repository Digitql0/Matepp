#include <iostream>

#include "board.hpp"

int main() {
  std::cout << "Hello World!" << std::endl;

  game board = makeGame();
  readBoard(board.board, false);

  return 0;
}

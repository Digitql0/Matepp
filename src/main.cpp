#include <iostream>
#include <utility>

#include "board.hpp"

int main() {
  std::cout << "Program Started." << std::endl;

  game board;

  std::string input;
  while (true) {
    board.readBoard(false);
    std::cout << "> ";
    if (!std::getline(std::cin, input)) break;

    if (input == "exit" || input == "quit") break;

    if (input == "change") board.changeColor();

    if (input.size() >= 4) {
      board.makeMove(input.substr(0, 2), input.substr(2, 4));
    }

    if (input.size() == 2) {
      board.showMoves(input);
    }

    if (input.size() == 1) {
      board.promote(input);
    }
  }

  return 0;
}

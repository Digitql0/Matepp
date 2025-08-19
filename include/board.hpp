#ifndef BOARD_HPP
#define BOARD_HPP

#define TYPE 0b00000111
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6
#define NONE 7

#define COLOR 0b00011000
#define WHITE 0b00001000
#define BLACK 0b00010000

#define FLAGS       0b01100000
#define MOVED       0b00100000
#define DOUBLESTEP  0b01000000

#define TURN 1

#include <string>
#include <vector>
#include <iostream>

class game {
public:
  game();
  void readBoard(bool id);
  void makeMove(std::string a, std::string b);
  bool isMoveLegal(int fr, int fc, int sr, int sc, bool& isEnpassant);
  void changeColor();
  void applyMove(int firstRow, int firstColumn, int secondRow, int secondColumn);
  void promote(std::string piece);
  void showMoves(std::string cell);
  bool isWhite;
  bool isWhitesTurn;

private:
  std::vector<std::vector<int>> board;
  std::vector<std::pair<int, int>> moves;

  std::pair<int, int> selectedCell;
  bool cellSelected;

  int whiteDirection;

  std::pair<int, int> pendingPromotionCell;
  bool pendingPromotion;
  std::vector<std::pair<int, int>> getPossibleMoves(int fr, int fc, int cell);
  void getRowColumn(std::string cell, int &row, int &column, bool isWhite);
  char getPieceChar(int pieceType);
  int getPieceType(char pieceChar);
  void checkCheckmate();
};

extern std::vector<int> pawnRow;
extern std::vector<int> kingRow;
extern std::vector<int> emptyRow;
extern std::vector<std::pair<int, char>> pieceCharPairs;

#endif

#include "board.hpp"

#include <algorithm>
#include <string>

#include "utility.hpp"

std::vector<int> pawnRow = {PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN};
std::vector<int> kingRow = {ROOK,  KNIGHT, BISHOP, KING,
                            QUEEN, BISHOP, KNIGHT, ROOK};
std::vector<int> emptyRow = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};
std::vector<std::pair<int, char>> pieceCharPairs = {
    {NONE, ' '}, {PAWN, 'P'},  {KNIGHT, 'k'}, {BISHOP, 'B'},
    {ROOK, 'R'}, {QUEEN, 'Q'}, {KING, 'K'}};

game::game()
    : isWhite(true),
      isWhitesTurn(true),
      cellSelected(false),
      whiteDirection(-1),
      pendingPromotion(false) {
  for (size_t i = 0; i < 8; i++) {  // ROW
    std::vector<int> column;
    if (i == 0 || i == 7) {
      column = kingRow;
    } else if (i == 1 || i == 6) {
      column = pawnRow;
    } else {
      column = emptyRow;
    }

    for (size_t j = 0; j < 8; j++) {  // COLUMN
      if (i == 0 || i == 1) {
        setBit(column[j], BLACK);
      }
      if (i == 6 || i == 7) {
        setBit(column[j], WHITE);
      }
    }

    board.push_back(column);
  }
}

void game::readBoard(bool id) {
  for (size_t i = 0; i < board.size(); i++) {  // ROW
    if (isWhite)
      std::cout << "[" << 8 - i << "]";
    else
      std::cout << "[" << i + 1 << "]";
    for (size_t j = 0; j < board[i].size(); j++) {  // COLUMN

      char pieceChar;
      int pieceType = (board[i][j] & TYPE);
      bool move = false;
      bool capture = false;

      if (cellSelected) {
        int selectedRow = selectedCell.first;
        int selectedColumn = selectedCell.second;
        auto selectedType = (board[selectedRow][selectedColumn] & TYPE);
        for (auto& [row, column] : moves) {
          if (row != i || column != j) continue;
          move = true;

          // Normal Capture
          if (pieceType != NONE) capture = true;

          // En Passant Capture
          if (selectedType == PAWN && pieceType == NONE &&
              selectedColumn != column)
            capture = true;
        }
      }

      if (capture)
        pieceChar = '@';
      else if (move)
        pieceChar = '#';
      else if (id)
        pieceChar = char(board[i][j]);
      else
        pieceChar = getPieceChar(pieceType);

      std::cout << "[" << pieceChar << "]";
    }
    std::cout << std::endl;
  }

  // BOTTOM OF THE BOARD
  char turnChar = isWhitesTurn ? 'W' : 'B';
  char sideChar = isWhite ? 'W' : 'B';
  std::cout << "[" << turnChar << "][A][B][C][D][E][F][G][H][" << sideChar
            << "]" << std::endl;
}

void game::makeMove(std::string a, std::string b) {
  // GET START AND DESTINATION OF MOVE
  int firstRow;
  int firstColumn;
  int secondRow;
  int secondColumn;
  getRowColumn(a, firstRow, firstColumn, isWhite);
  getRowColumn(b, secondRow, secondColumn, isWhite);
  auto& first = board[firstRow][firstColumn];
  auto& second = board[secondRow][secondColumn];

  // Debugging
  bool white = (board[firstRow][firstColumn] & COLOR) == WHITE;
  std::cout << "COLOR: " << white << std::endl;
  std::cout << "TURN: " << isWhitesTurn << std::endl;
  std::cout << "LEGAL WHITE: " << (isWhitesTurn && white) << std::endl;
  std::cout << "LEGAL BLACK: " << (!isWhitesTurn && !white) << std::endl;

  char firstChar = getPieceChar(first & TYPE);
  char secondChar = getPieceChar(second & TYPE);
  std::cout << "[" << firstChar << "] -> [" << secondChar << "]" << std::endl;

  bool isEnpassant = false;
  auto enpassantCell = board[firstRow][secondColumn];
  bool isLegalMove =
      isMoveLegal(firstRow, firstColumn, secondRow, secondColumn, isEnpassant);

  std::cout << "LEGAL: " << isLegalMove << std::endl;
  if (!isLegalMove) return;
  if (isEnpassant)
    applyMove(firstRow, firstColumn, firstRow, secondColumn);
  else
    applyMove(firstRow, firstColumn, secondRow, secondColumn);
}

std::vector<std::pair<int, int>> game::getPossibleMoves(int fr, int fc,
                                                        int cell) {
  std::vector<std::pair<int, int>> moves;
  int cellColor = cell & COLOR;
  excludeBits(cell, TYPE);

  // pawn direction
  int dir = whiteDirection;

  if (cell == PAWN) {
    // FORWARD
    int sr = fr;
    int sc = fc;
    for (int i = 0; i < 2; i++) {
      sr += dir;
      if (sr < 0 || sr >= 8) break;
      moves.push_back({sr, sc});
      if ((board[sr][sc] & TYPE) != NONE) break;
    }

    // CAPTURE
    sr = fr + dir;
    for (auto d : {-1, 1}) {
      sc = fc + d;
      if (sc < 0 || sc >= 8 || sr < 0 || sr >= 8) continue;
      moves.push_back({sr, sc});
    }
  } else if (cell == KNIGHT) {
    for (auto d1 : {-1, 1}) {
      for (auto d2 : {-2, 2}) {
        int sr = -1;
        int sc = -1;

        // Horizontal Displacement
        sr = fr + d1;
        sc = fc + d2;
        if (sr >= 0 && sr < 8 && sc >= 0 && sc < 8) moves.push_back({sr, sc});

        // Vertical Displacement
        sr = fr + d2;
        sc = fc + d1;
        if (sr >= 0 && sr < 8 && sc >= 0 && sc < 8 && board[sr][sc] == NONE)
          moves.push_back({sr, sc});
      }
    }
  } else if (cell == BISHOP) {
    for (auto d1 : {-1, 1}) {
      for (auto d2 : {-1, 1}) {
        int sr = fr;
        int sc = fc;
        while (true) {
          sr += d1;
          sc += d2;
          if (sr >= 8 || sr < 0 || sc >= 8 || sc < 0) break;
          moves.push_back({sr, sc});
          if ((board[sr][sc] & TYPE) != NONE) break;
        }
      }
    }
  } else if (cell == ROOK) {
    for (auto d1 : {-1, 1}) {
      int sr = fr;
      int sc = fc;
      bool rlock = false;
      bool clock = false;
      while (true) {
        sr += d1;
        sc += d1;
        bool roob = sr < 0 || sr >= 8;
        bool coob = sc < 0 || sc >= 8;
        if ((roob || rlock) && (clock || coob)) break;
        if (!roob && !rlock) {
          moves.push_back({sr, fc});
          if ((board[sr][fc] & TYPE) != NONE) rlock = true;
        }
        if (!coob && !clock) {
          moves.push_back({fr, sc});
          if ((board[fr][sc] & TYPE) != NONE) clock = true;
        }
      }
    }
  } else if (cell == QUEEN) {
    for (auto d1 : {-1, 1}) {
      for (auto d2 : {-1, 1}) {
        int sr = fr;
        int sc = fc;
        while (true) {
          sr += d1;
          sc += d2;
          if (sr >= 8 || sr < 0 || sc >= 8 || sc < 0) break;
          moves.push_back({sr, sc});
          if ((board[sr][sc] & TYPE) != NONE) break;
        }
      }
    }
    for (auto d1 : {-1, 1}) {
      int sr = fr;
      int sc = fc;
      bool rlock = false;
      bool clock = false;
      while (true) {
        sr += d1;
        sc += d1;
        bool roob = sr < 0 || sr >= 8;
        bool coob = sc < 0 || sc >= 8;
        if ((roob || rlock) && (clock || coob)) break;
        if (!roob && !rlock) {
          moves.push_back({sr, fc});
          if ((board[sr][fc] & TYPE) != NONE) rlock = true;
        }
        if (!coob && !clock) {
          moves.push_back({fr, sc});
          if ((board[fr][sc] & TYPE) != NONE) clock = true;
        }
      }
    }
  } else if (cell == KING) {
    for (auto d1 : {-1, 0, 1}) {
      int sr = fr + d1;
      if (sr < 0 || sr >= 8) continue;
      for (auto d2 : {-1, 0, 1}) {
        int sc = fc + d2;
        if (sc < 0 || sc >= 8) continue;
        if (sc == fc && sr == fr) continue;
        moves.push_back({sr, sc});
      }
    }
  }

  return moves;
}

void game::showMoves(std::string cell) {
  int firstColumn;
  int firstRow;
  getRowColumn(cell, firstRow, firstColumn, isWhite);

  moves = getPossibleMoves(firstRow, firstColumn, board[firstRow][firstColumn]);
  selectedCell = {firstRow, firstColumn};
  cellSelected = true;

  // DEBUGGING
  {
    std::cout << firstRow << ", " << firstColumn << std::endl;
    for (auto& [r, c] : moves) {
      std::cout << "[" << r << ", " << c << "]";
    }
    std::cout << std::endl;
    int piece = board[firstRow][firstColumn];
    int pieceType = (piece & TYPE);
    char pieceChar = getPieceChar(pieceType);
    std::cout << firstRow << ", " << firstColumn << ": [" << pieceChar
              << "] WHITE: " << (piece & WHITE)
              << " DOUBLESTEP: " << (piece & DOUBLESTEP)
              << " MOVED: " << (piece & MOVED) << " piece: " << piece
              << std::endl;
  }
}

void game::getRowColumn(std::string cell, int& row, int& column, bool isWhite) {
  row = 0;
  column = 0;

  // Error Checks
  if (cell.size() < 2) {
    return;
  }
  if (!std::isalpha(cell[0]) || !std::isdigit(cell[1])) {
    return;
  }

  // Set Variables
  if (isWhite) {
    column = std::tolower(cell[0]) - 97;
    row = 8 - (cell[1] - 49 + 1);
  } else {
    column = 7 - (std::tolower(cell[0]) - 97);
    row = cell[1] - 49;
  }
  std::cout << "X: " << column << " Y: " << row << std::endl;

  // Bound Checks
  if (column > 7 || row > 7) {
    row = 0;
    column = 0;
    return;
  }
}

char game::getPieceChar(int pieceType) {
  for (const auto& [type, ch] : pieceCharPairs) {
    if (pieceType == type) return ch;
  }

  return ' ';
}

int game::getPieceType(char pieceChar) {
  for (const auto& [type, ch] : pieceCharPairs) {
    if (pieceChar == ch) return type;
  }

  return -1;
}

void game::changeColor() {
  isWhite = !isWhite;
  whiteDirection *= -1;

  // ROTATE BOARD
  std::reverse(board.begin(), board.end());

  for (auto& row : board) {
    std::reverse(row.begin(), row.end());
  }

  auto& selFirst = selectedCell.first;
  auto& selSecond = selectedCell.second;
  auto& selCell = board[selFirst][selSecond];

  // ROTATED SELECTED CELL
  selFirst = 7 - selFirst;
  selSecond = 7 - selSecond;

  moves = getPossibleMoves(selFirst, selSecond, selCell);
}

void game::promote(std::string piece) {
  if (!pendingPromotion) return;

  char pieceChar = piece[0];
  int pieceBitMask = getPieceType(pieceChar);

  if (pieceBitMask == 0) return;

  auto& promotionCell =
      board[pendingPromotionCell.first][pendingPromotionCell.second];
  removeBit(promotionCell, TYPE);
  setBit(promotionCell, pieceBitMask);
  pendingPromotion = false;

  isWhitesTurn = !isWhitesTurn;

  moves.clear();
}

void game::checkCheckmate() {}

bool game::isMoveLegal(int fr, int fc, int sr, int sc, bool& isEnpassant) {
  auto legalMoves = getPossibleMoves(fr, fc, board[fr][fc]);
  bool isLegalMove = false;

  for (auto& [row, column] : legalMoves) {
    if (row == sr && column == sc) {
      isLegalMove = true;  // Is the Move even theoretically possible
      break;
    }
  }

  auto& first = board[fr][fc];
  auto& second = board[sr][sc];

  bool isWhitePiece = (first & COLOR) == WHITE;
  bool legalBlackMove = (!isWhitesTurn && !isWhitePiece);
  bool legalWhiteMove = (isWhitesTurn && isWhitePiece);

  // More Concrete checking if it is actually possible
  if (!legalWhiteMove && !legalBlackMove) isLegalMove = false;
  if (pendingPromotion) isLegalMove = false;

  // Attacking same color pieces
  int firstColor = first;
  int secondColor = second;
  excludeBits(fc, COLOR);
  excludeBits(sc, COLOR);
  if (fc != 0 && sc != 0 && fc == sc) isLegalMove = false;

  // ENPASSANT AND FALSE CAPTURE
  auto& enpassant = board[fr][sc];
  isEnpassant = false;
  bool isPawn = (first & TYPE) == PAWN;
  bool takesNone = (second & TYPE) == NONE || (second & COLOR) == fc;
  bool diffTypes = isPawn && takesNone;
  bool enpassantTaken = (enpassant & TYPE) != NONE &&
                        (enpassant & COLOR) != fc &&
                        (enpassant & DOUBLESTEP) != 0;
  bool diffCols = fc != sc;

  if (isPawn && diffCols) {
    if (!takesNone)
      isLegalMove = true;
    else if (takesNone && !enpassantTaken)
      isLegalMove = false;
    else
      isEnpassant = true;
  }

  return isLegalMove;
}

void game::applyMove(int firstRow, int firstColumn, int secondRow,
                     int secondColumn) {
  auto& first = board[firstRow][firstColumn];
  auto firstType = first & TYPE;
  auto& second = board[secondRow][secondColumn];
  auto secondType = second & TYPE;
  auto secondColor = second & COLOR;

  second = first;
  first = NONE;
  removeBit(second, DOUBLESTEP);
  setBit(second, MOVED);

  // check if pawn did a double step
  if (abs(firstRow - secondRow) == 2 && firstType == PAWN)
    setBit(second, DOUBLESTEP);

  // Check for Promotion
  if (secondType == PAWN) {
    int lastRow =
        (secondColor != BLACK) ? (whiteDirection * 7) : (-whiteDirection * 7);
    lastRow = std::clamp(lastRow, 0, 7);

    if (secondRow == lastRow) {
      pendingPromotion = true;
      pendingPromotionCell = {secondRow, secondColumn};
    }
  }

  isWhitesTurn = !isWhitesTurn;

  moves.clear();

  checkCheckmate();
}

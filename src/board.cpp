#include "board.hpp"

#include <algorithm>
#include <string>

#include "utility.hpp"

std::vector<int> pawn_row = {PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN, PAWN};
std::vector<int> king_row = {ROOK,  KNIGHT, BISHOP, KING,
                             QUEEN, BISHOP, KNIGHT, ROOK};
std::vector<int> empty_row = {NONE, NONE, NONE, NONE, NONE, NONE, NONE, NONE};

game::game() {
  for (size_t i = 0; i < 8; i++) {  // ROW
    std::vector<int> column;
    if (i == 0 || i == 7) {
      column = king_row;
    } else if (i == 1 || i == 6) {
      column = pawn_row;
    } else {
      column = empty_row;
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

  isWhite = true;
  isWhitesTurn = true;
  selected_cell = {0, 0};
  cell_selected = false;
  whiteDirection = -1;
  pending_promotion = false;
  pending_promotion_cell = {0, 0};
}

void game::readBoard(bool id) {
  for (size_t i = 0; i < board.size(); i++) {  // ROW
    if (isWhite)
      std::cout << "[" << 8 - i << "]";
    else
      std::cout << "[" << i + 1 << "]";
    for (size_t j = 0; j < board[i].size(); j++) {  // COLUMN

      char pc;                        // piece char
      int pt = (board[i][j] & TYPE);  // piece type
      bool move = false;
      bool capture = false;

      if (cell_selected) {
        int sr = selected_cell.first;      // selected row
        int sc = selected_cell.second;     // selected column
        auto st = (board[sr][sc] & TYPE);  // selected type
        for (auto& [row, col] : moves) {
          if (row != i || col != j) continue;
          move = true;

          // Normal Capture
          if (pt != NONE) capture = true;

          // En Passant Capture
          if (st == PAWN && pt == NONE && sc != col) capture = true;
        }
      }

      if (capture)
        pc = '@';
      else if (move)
        pc = '#';
      else if (id)
        pc = char(board[i][j]);
      else
        getPieceChar(i, j, pc);

      std::cout << "[" << pc << "]";
    }
    std::cout << std::endl;
  }

  // BOTTOM OF THE BOARD
  char turn = ' ';
  if (isWhitesTurn)
    turn = 'W';
  else
    turn = 'B';

  if (isWhite)
    std::cout << "[" << turn << "][A][B][C][D][E][F][G][H] [W]" << std::endl;
  else
    std::cout << "[" << turn << "][H][G][F][E][D][C][B][A] [B]" << std::endl;
}

void game::makeMove(std::string a, std::string b) {
  int first_row;
  int first_column;
  int second_row;
  int second_column;
  getRowColumn(a, first_row, first_column, isWhite);
  getRowColumn(b, second_row, second_column, isWhite);
  auto& first = board[first_row][first_column];
  auto& second = board[second_row][second_column];

  bool white = (board[first_row][first_column] & COLOR) == WHITE;
  std::cout << "COLOR: " << white << std::endl;
  std::cout << "TURN: " << isWhitesTurn << std::endl;
  std::cout << "LEGAL WHITE: " << (isWhitesTurn && white) << std::endl;
  std::cout << "LEGAL BLACK: " << (!isWhitesTurn && !white) << std::endl;

  // Debugging
  char first_pc;
  char second_pc;
  getPieceChar(first_row, first_column, first_pc);
  getPieceChar(first_row, first_column, second_pc);
  std::cout << "[" << first_pc << "] -> [" << second_pc << "]" << std::endl;

  auto legalMoves =
      getPossibleMoves(first_row, first_column, first, whiteDirection);
  bool isLegalMove = false;
  {
    for (auto& [row, col] : legalMoves) {
      if (row == second_row && col == second_column) {
        isLegalMove = true;
        break;
      }
    }
  }

  if (!((isWhitesTurn && white) || (!isWhitesTurn && !white)))
    isLegalMove = false;
  if (pending_promotion) isLegalMove = false;
  int fc = first;
  int sc = second;
  excludeBits(fc, COLOR);
  excludeBits(sc, COLOR);
  if (fc != 0 && sc != 0 && fc == sc) isLegalMove = false;

  auto& enpassant = board[first_row][second_column];
  bool isEnpassant = false;
  bool isPawn = (first & TYPE) == PAWN;
  bool takesNone = (second & TYPE) == NONE || (second & COLOR) == fc;
  bool diffTypes = isPawn && takesNone;
  bool enpassantTaken = (enpassant & TYPE) != NONE &&
                        (enpassant & COLOR) != fc &&
                        (enpassant & DOUBLESTEP) != 0;
  bool diffCols = first_column != second_column;

  if (isPawn && diffCols) {
    if (!takesNone)
      isLegalMove = true;
    else if (takesNone && !enpassantTaken)
      isLegalMove = false;
    else
      isEnpassant = true;
  }

  std::cout << "LEGAL: " << isLegalMove << std::endl;
  if (!isLegalMove) return;

  second = first;
  if (isEnpassant) enpassant = NONE;
  first = NONE;
  removeBit(second, DOUBLESTEP);
  setBit(second, MOVED);

  // check if pawn did a double step
  if (abs(first_row - second_row) == 2 && (first & TYPE) == PAWN)
    setBit(second, DOUBLESTEP);

  // Check for Promotion
  if ((second & TYPE) == PAWN) {
    int lastRow =
        ((second & WHITE) != 0) ? (whiteDirection * 7) : (-whiteDirection * 7);
    lastRow = std::clamp(lastRow, 0, 7);

    if (second_row == lastRow) {
      pending_promotion = true;
      pending_promotion_cell = {second_row, second_column};
    }
  }

  isWhitesTurn = !isWhitesTurn;

  moves.clear();

  checkCheckmate();
}

std::vector<std::pair<int, int>> game::getPossibleMoves(int fr, int fc,
                                                        int cell,
                                                        int whiteDirection) {
  std::vector<std::pair<int, int>> moves;
  int type = board[fr][fc];
  int color = board[fr][fc];
  int flags = board[fr][fc];
  excludeBits(type, TYPE);
  excludeBits(color, COLOR);
  excludeBits(flags, MOVED | DOUBLESTEP);
  excludeBits(cell, TYPE);

  // pawn direction
  int dir = whiteDirection;

  if (color != WHITE) dir *= -1;

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
      // bool enpassant =
      //     (board[fr][sc] & TYPE) != NONE && (board[fr][sc] & DOUBLESTEP) !=
      //     0;
      // bool capture = (board[sr][sc] & TYPE) != NONE;
      // if (capture || enpassant) {
      moves.push_back({sr, sc});
      // }
    }

    // // One Forward
    // int newRow = fr + dir;
    // if (newRow >= 0 && newRow < 8 && (board[newRow][fc] & TYPE) == NONE) {
    //   moves.push_back({newRow, fc});
    //   // double step
    //   newRow += dir;
    //   if (!hasBit(flags, MOVED) && newRow >= 0 && newRow < 8 &&
    //       (board[newRow][fc] & TYPE) == NONE) {
    //     moves.push_back({newRow, fc});
    //   }
    // }

    // // CAPTURING
    // for (int deltaColumn : {-1, 1}) {
    //   int newRow = fr + dir;
    //   int newColumn = fc + deltaColumn;
    //   if (newColumn < 0 || newColumn >= 8 || newRow < 0 || newRow >= 8)
    //     continue;
    //   auto& cell = board[newRow][newColumn];
    //   auto& enpassant = board[fr][newColumn];
    //   if ((cell & TYPE) != NONE) {
    //     moves.push_back({newRow, newColumn});
    //   }
    //   if ((enpassant & TYPE) == PAWN &&
    //       (enpassant & DOUBLESTEP) == DOUBLESTEP && (enpassant & color) == 0)
    //       {
    //     moves.push_back({newRow, newColumn});
    //   }
    // }
  }
  if (cell == KNIGHT) {
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
  }
  if (cell == BISHOP) {
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
  }
  if (cell == ROOK) {
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
  }
  if (cell == QUEEN) {
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
  }
  if (cell == KING) {
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
  int first_column;
  int first_row;
  getRowColumn(cell, first_row, first_column, isWhite);
  std::cout << first_row << ", " << first_column << std::endl;
  moves = getPossibleMoves(first_row, first_column,
                           board[first_row][first_column], whiteDirection);
  for (auto& [r, c] : moves) {
    std::cout << "[" << r << ", " << c << "]";
  }
  selected_cell = {first_row, first_column};
  cell_selected = true;
  std::cout << std::endl;

  // DEBUGGING
  int piece = board[first_row][first_column];
  char pc;
  getPieceChar(first_row, first_column, pc);
  std::cout << first_row << ", " << first_column << ": [" << pc
            << "] WHITE: " << (piece & WHITE)
            << " DOUBLESTEP: " << (piece & DOUBLESTEP)
            << " MOVED: " << (piece & MOVED) << " piece: " << piece
            << std::endl;
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

void game::getPieceChar(int row, int column, char& pc) {
  if (row >= 8 || row < 0 || column >= 8 || column < 0) return;

  int pt = board[row][column];  // piece type
  excludeBits(pt, TYPE);
  switch (pt) {
    case PAWN:
      pc = 'P';
      break;
    case KNIGHT:
      pc = 'k';
      break;
    case BISHOP:
      pc = 'B';
      break;
    case ROOK:
      pc = 'R';
      break;
    case QUEEN:
      pc = 'Q';
      break;
    case KING:
      pc = 'K';
      break;
    case NONE:
      pc = ' ';
      break;
  }
}

void game::changeColor() {
  isWhite = !isWhite;
  whiteDirection *= -1;

  std::cout << "Board size: " << board.size() << "\n";
  for (size_t i = 0; i < board.size(); ++i) {
    std::cout << "Row " << i << " size: " << board[i].size() << "\n";
  }

  // ROTATE BOARD
  std::reverse(board.begin(), board.end());

  for (auto& row : board) {
    std::reverse(row.begin(), row.end());
  }

  // // ROTATED SELECTED CELL
  // selected_cell.first = 7 - selected_cell.first;
  // selected_cell.second = 7 - selected_cell.second;

  // moves = getPossibleMoves(board, selected_cell.first,
  //                               selected_cell.second,
  //                               whiteDirection);
}

void game::promote(std::string piece) {
  std::cout << "PROMOTION: " << pending_promotion;
  if (!pending_promotion) return;

  int pieceBitMask = 0;
  char pieceChar = piece[0];
  switch (pieceChar) {
    case 'k':
      pieceBitMask = KNIGHT;
      break;
    case 'B':
      pieceBitMask = BISHOP;
      break;
    case 'K':
      pieceBitMask = KING;
      break;
    case 'R':
      pieceBitMask = ROOK;
      break;
    case 'Q':
      pieceBitMask = QUEEN;
      break;
  }

  std::cout << "CHAR: " << pieceChar << " BITMASK: " << pieceBitMask
            << std::endl;

  if (pieceBitMask == 0) return;

  auto& cell =
      board[pending_promotion_cell.first][pending_promotion_cell.second];
  removeBit(cell, TYPE);
  setBit(cell, pieceBitMask);
  pending_promotion = false;

  isWhitesTurn = !isWhitesTurn;

  moves.clear();
}

void game::checkCheckmate() {}

#include "../headers/board.h"
#include <iostream>
#include <random>

Board::Board() : board(64, Piece::None), sideToMove(Piece::White),
              canCastleKingsideWhite(false), canCastleQueensideWhite(false),
              canCastleKingsideBlack(false), canCastleQueensideBlack(false),
              enPassantTarget(-1) {}

#ifndef EVAL_H
#define EVAL_H

#include "board.h"

class Evaluator {
public:
    static int evaluate(const Board& board);

    static const int PAWN_VALUE = 100;
    static const int KNIGHT_VALUE = 320;
    static const int BISHOP_VALUE = 330;
    static const int ROOK_VALUE = 500;
    static const int QUEEN_VALUE = 900;
    static const int KING_VALUE = 20000;

    static int piecePositionalValue(const Piece& piece, int square);
    static int pieceValue(const Piece& piece);

    static bool isWhite(const Piece& piece);
    static bool isBlack(const Piece& piece);
};

#endif

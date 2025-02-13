#ifndef MOVE_H
#define MOVE_H

#include "piece.h"

struct Move {
    int from;
    int to;
    int capturePiece;
    bool isPromotion;
    bool isEnPassant;
    int promotionPiece;
    bool isCastling;

    Move(int f, int t, int cap, bool promo, bool ep, int promoPiece, bool castle)
        : from(f), to(t), capturePiece(cap), isPromotion(promo), isEnPassant(ep),
          promotionPiece(promoPiece), isCastling(castle) {}

    Move() : from(-1), to(-1), capturePiece(Piece::None), isPromotion(false),
             isEnPassant(false), promotionPiece(Piece::None), isCastling(false) {}
};

struct MoveHistory {
    Move move;
    int capturedPiece;           
    int movedPiece;           
    int oldEnPassant;         
    bool oldCanCastleKingsideWhite;
    bool oldCanCastleQueensideWhite;
    bool oldCanCastleKingsideBlack;
    bool oldCanCastleQueensideBlack;

    int rookFrom = -1;
    int rookTo = -1;
    int rookPiece = Piece::None;
};

#endif
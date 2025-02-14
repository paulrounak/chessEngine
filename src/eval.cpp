#include "../headers/eval.h"
#include <cmath>
#include <algorithm>

int Evaluator::piecePositionalValue(const Piece& piece, int square) {
    int file = square % 8;
    int rank = square / 8;

    double dist = std::abs(file - 3.5) + std::abs(rank - 3.5);
    int bonus = static_cast<int>(7 - dist);

    switch(piece.getType()) {
        case Piece::Knight:
            bonus = bonus * 2 + 3;
            break;
        case Piece::Bishop:
            bonus = bonus * 2 + 5;
            break;
        case Piece::Pawn:
            bonus = bonus / 2;
            break;
        case Piece::King:
            bonus = 0;
            break;
        default:
            break;
    }
    return bonus;
}

int Evaluator::evaluate(const Board& board) {
    int score = 0;

    for (int i = 0; i < 64; ++i) {
        Piece piece = board.getPieceAt(i);
        if (piece.getType() != Piece::None) {
            int material   = pieceValue(piece);
            int positional = piecePositionalValue(piece, i);
            int pieceScore = material + positional;
            if (isWhite(piece))
                score += pieceScore;
            else
                score -= pieceScore;
        }
    }
    
    if (board.moveCount < 20) {
        for (int i = 0; i < 64; ++i) {
            Piece piece = board.getPieceAt(i);
            if (piece.getType() == Piece::Pawn) {
                int file = i % 8;
                int rank = i / 8;
                if (piece.getColor() == Piece::White && rank < 6 && (file == 3 || file == 4))
                    score += 20;
                if (piece.getColor() == Piece::Black && rank > 1 && (file == 3 || file == 4))
                    score -= 20;
            }
        }
    }
    
    return score;
}

int Evaluator::pieceValue(const Piece& piece) {
    switch (piece.getType()) {
        case Piece::King:   return KING_VALUE;
        case Piece::Queen:  return QUEEN_VALUE;
        case Piece::Rook:   return ROOK_VALUE;
        case Piece::Bishop: return BISHOP_VALUE;
        case Piece::Knight: return KNIGHT_VALUE;
        case Piece::Pawn:   return PAWN_VALUE;
        default: return 0;
    }
}

bool Evaluator::isWhite(const Piece& piece) {
    return piece.getColor() == Piece::White;
}

bool Evaluator::isBlack(const Piece& piece) {
    return piece.getColor() == Piece::Black;
}
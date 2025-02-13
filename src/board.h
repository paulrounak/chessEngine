#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>
#include "piece.h"
#include "move.h"

class Board {
public:
    std::vector<int> board;
    int sideToMove;
    bool canCastleKingsideWhite, canCastleQueensideWhite;
    bool canCastleKingsideBlack, canCastleQueensideBlack;
    int enPassantTarget;

    Board();
    void fenPosition(const std::string& fen);
    std::vector<Move> generatePawnMoves(int square, int color);
    std::vector<Move> generateKnightMoves(int square, int color);
    std::vector<Move> generateRookMoves(int square, int color);
    std::vector<Move> generateBishopMoves(int square, int color);
    std::vector<Move> generateQueenMoves(int square, int color);
    std::vector<Move> generateKingMoves(int square, int color, bool canCastleK, bool canCastleQ);
    bool isSquareAttacked(int square, int attackerColor);
    bool isKingInCheck(int color);
    std::vector<Move> generateLegalMoves();
    void makeMove(const Move& move);
    void unmakeMove();
    int moveGenerationTest(int depth);

private:
    std::vector<MoveHistory> moveHistoryStack;
};

#endif
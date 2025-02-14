#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "move.h"
#include "eval.h"

class Search {
public:
    Search(Board& board, int depth);
    Move findBestMove();

private:
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer); 
    int evaluateBoard(Board& board);
    bool isGameOver(Board& board);

    Move findBestMoveAtDepth(int currentDepth);
    int moveHeuristic(const Move& move);

    Board& board; 
    int depth;
};

#endif

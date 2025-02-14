#include "../headers/search.h"
#include <limits>
#include <algorithm>
#include <iostream>

Search::Search(Board& board, int depth) : board(board), depth(depth) {}

Move Search::findBestMove() {
    Move bestMove;
    // Iterative deepening: start at depth 1 and increase to maxDepth.
    for (int currentDepth = 1; currentDepth <= depth; ++currentDepth) {
        bestMove = findBestMoveAtDepth(currentDepth);
        std::cout << "Completed search at depth " << currentDepth << std::endl;
    }
    return bestMove;
}

Move Search::findBestMoveAtDepth(int currentDepth) {
    bool maximizing = (board.sideToMove == Piece::White);
    int bestScore = maximizing ? -std::numeric_limits<int>::max() 
                               : std::numeric_limits<int>::max();
    Move bestMove;
    
    // Get legal moves and sort them using move heuristic.
    std::vector<Move> legalMoves = board.generateLegalMoves();
    std::sort(legalMoves.begin(), legalMoves.end(), [this](const Move& a, const Move& b) {
        return moveHeuristic(a) > moveHeuristic(b);
    });
    
    for (const Move& move : legalMoves) {
        board.makeMove(move);
        int score = minimax(board, currentDepth - 1,
                            -std::numeric_limits<int>::max(),
                            std::numeric_limits<int>::max(),
                            !maximizing);
        board.unmakeMove();
        
        if (maximizing) {
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
        } else {
            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
        }
    }
    
    return bestMove;
}

int Search::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || isGameOver(board)) {
        return evaluateBoard(board);
    }
    
    std::vector<Move> legalMoves = board.generateLegalMoves();
    if (maximizingPlayer) {
        int maxEval = -std::numeric_limits<int>::max();
        for (const Move& move : legalMoves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.unmakeMove();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Beta cutoff
        }
        return maxEval;
    } else {
        int minEval = std::numeric_limits<int>::max();
        for (const Move& move : legalMoves) {
            board.makeMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.unmakeMove();
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Alpha cutoff
        }
        return minEval;
    }
}

int Search::evaluateBoard(Board& board) {
    if (isGameOver(board)) {
        if (board.isKingInCheck(board.sideToMove)) {
            // Checkmate: return a huge value based on which side is in check
            return (board.sideToMove == Piece::White) 
                   ? -Evaluator::KING_VALUE * 1000 
                   : Evaluator::KING_VALUE * 1000;
        } else {
            // Stalemate
            return 0;
        }
    }
    return Evaluator::evaluate(board);
}

bool Search::isGameOver(Board& board) {
    return board.generateLegalMoves().empty();
}

int Search::moveHeuristic(const Move& move) {
    int score = 0;
    
    // MVV-LVA capture bonus:
    Piece captured = board.getPieceAt(move.to);
    if (captured.getType() != Piece::None) {
        score += Evaluator::pieceValue(captured) - Evaluator::pieceValue(board.getPieceAt(move.from));
    }
    
    Piece movingPiece = board.getPieceAt(move.from);
    int fromRank = move.from / 8;
    int toRank   = move.to / 8;
    int toFile   = move.to % 8;
    
    if (movingPiece.getType() == Piece::Pawn) {
        if (toFile == 3 || toFile == 4)
            score += 20;
        
        if ((movingPiece.getColor() == Piece::White && toRank < fromRank) || // White pawns move upward
            (movingPiece.getColor() == Piece::Black && toRank > fromRank))  // Black pawns move downward
            score += 15;
    }
    
    else if (movingPiece.getType() == Piece::Knight) {
        if (toFile == 3 || toFile == 4)
            score += 10;
    }
    
    if (movingPiece.getType() == Piece::King && abs(move.from - move.to) == 2)
        score += 50;
    
    return score;
}
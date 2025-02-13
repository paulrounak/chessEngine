#include "../headers/board.h"

// Generate legal moves by filtering out pseudo–moves that leave the king in check.
std::vector<Move> Board::generateLegalMoves() {
    std::vector<Move> legalMoves;
    std::vector<Move> pseudoMoves;

    for (int i = 0; i < 64; i++) {
        // Only consider pieces of the side to move.
        if (board[i] == Piece::None || ((board[i] & (Piece::White | Piece::Black)) != sideToMove))
            continue;
        int pieceType = board[i] & 7;
        switch (pieceType) {
            case Piece::Pawn:
                pseudoMoves = generatePawnMoves(i, sideToMove);
                break;
            case Piece::Knight:
                pseudoMoves = generateKnightMoves(i, sideToMove);
                break;
            case Piece::Bishop:
                pseudoMoves = generateBishopMoves(i, sideToMove);
                break;
            case Piece::Rook:
                pseudoMoves = generateRookMoves(i, sideToMove);
                break;
            case Piece::Queen:
                pseudoMoves = generateQueenMoves(i, sideToMove);
                break;
            case Piece::King: {
                bool canK = (sideToMove == Piece::White) ? canCastleKingsideWhite : canCastleKingsideBlack;
                bool canQ = (sideToMove == Piece::White) ? canCastleQueensideWhite : canCastleQueensideBlack;
                pseudoMoves = generateKingMoves(i, sideToMove, canK, canQ);
                break;
            }
            default:
                pseudoMoves.clear();
                break;
        }
        // For each pseudo–move, simulate it and check that the king isn’t left in check.
        for (Move move : pseudoMoves) {
            int fromPiece = board[move.from];
            int toPiece = board[move.to];
            int oldEnPassant = enPassantTarget;
            bool isEp = move.isEnPassant;
            int epPawnSquare = -1;
            bool isCastle = move.isCastling;
            int rookFrom = -1, rookTo = -1;
            int rookPiece = Piece::None; // for castling simulation

            // Make the move.
            board[move.from] = Piece::None;
            board[move.to] = fromPiece;
            if (isEp) {
                epPawnSquare = move.to + ((sideToMove == Piece::White) ? 8 : -8);
                board[epPawnSquare] = Piece::None;
            }
            if (isCastle) {
                if (move.to == move.from + 2) { // kingside
                    rookFrom = move.from + 3;
                    rookTo = move.from + 1;
                } else { // queenside
                    rookFrom = move.from - 4;
                    rookTo = move.from - 1;
                }
                rookPiece = board[rookFrom];
                board[rookFrom] = Piece::None;
                board[rookTo] = rookPiece;
            }

            bool inCheck = isKingInCheck(sideToMove);

            // Undo the move.
            board[move.from] = fromPiece;
            board[move.to] = toPiece;
            if (isEp)
                board[epPawnSquare] = move.capturePiece;
            if (isCastle) {
                board[rookFrom] = rookPiece;
                board[rookTo] = Piece::None;
            }
            enPassantTarget = oldEnPassant;

            if (!inCheck)
                legalMoves.push_back(move);
        }
    }
    return legalMoves;
}
#include "../headers/board.h"

void Board::makeMove(const Move& move) {
    MoveHistory history;
    history.move = move;
    history.oldEnPassant = enPassantTarget;
    history.oldCanCastleKingsideWhite = canCastleKingsideWhite;
    history.oldCanCastleQueensideWhite = canCastleQueensideWhite;
    history.oldCanCastleKingsideBlack = canCastleKingsideBlack;
    history.oldCanCastleQueensideBlack = canCastleQueensideBlack;
    
    history.movedPiece = board[move.from];
    history.capturedPiece = board[move.to];
    
    if (move.isCastling) {
        if (move.to == move.from + 2) { // kingside
            history.rookFrom = move.from + 3;
            history.rookTo = move.from + 1;
        } else { // queenside
            history.rookFrom = move.from - 4;
            history.rookTo = move.from - 1;
        }
        history.rookPiece = board[history.rookFrom];
    }
    
    int fromPiece = board[move.from];
    int color = fromPiece & (Piece::White | Piece::Black);
    int pieceType = fromPiece & 7;
    
    if (pieceType == Piece::King) {
        if (color == Piece::White) {
            canCastleKingsideWhite = false;
            canCastleQueensideWhite = false;
        } else {
            canCastleKingsideBlack = false;
            canCastleQueensideBlack = false;
        }
    } else if (pieceType == Piece::Rook) {
        if (color == Piece::White) {
            if (move.from == 7 * 8 + 7) 
                canCastleKingsideWhite = false;
            else if (move.from == 7 * 8 + 0) 
                canCastleQueensideWhite = false;
        } else {
            if (move.from == 0 * 8 + 7) 
                canCastleKingsideBlack = false;
            else if (move.from == 0 * 8 + 0) 
                canCastleQueensideBlack = false;
        }
    }
    
    if (move.capturePiece != Piece::None && ((move.capturePiece & 7) == Piece::Rook)) {
        int capturedSquare = move.to;
        int capturedColor = move.capturePiece & (Piece::White | Piece::Black);
        if (capturedColor == Piece::White) {
            if (capturedSquare == 7 * 8 + 7)
                canCastleKingsideWhite = false;
            else if (capturedSquare == 7 * 8 + 0)
                canCastleQueensideWhite = false;
        } else {
            if (capturedSquare == 0 * 8 + 7)
                canCastleKingsideBlack = false;
            else if (capturedSquare == 0 * 8 + 0)
                canCastleQueensideBlack = false;
        }
    }
    
    // Handle castling: move the rook.
    if (move.isCastling) {
        board[history.rookTo] = board[history.rookFrom];
        board[history.rookFrom] = Piece::None;
    }
    
    // Handle en passant.
    if (move.isEnPassant) {
        int capturedPawnSquare = move.to + ((color == Piece::White) ? 8 : -8);
        board[capturedPawnSquare] = Piece::None;
        enPassantTarget = -1;
    } else if (pieceType == Piece::Pawn && abs(move.to - move.from) == 16) {
        enPassantTarget = move.from + (move.to - move.from) / 2;
    } else {
        enPassantTarget = -1;
    }
    
    // Handle promotion.
    if (move.isPromotion)
        fromPiece = color | move.promotionPiece;
    
    // Make the move.
    board[move.from] = Piece::None;
    board[move.to] = fromPiece;

    moveCount++;
    
    sideToMove = (sideToMove == Piece::White) ? Piece::Black : Piece::White;
    
    this->moveHistoryStack.push_back(history);
}

void Board::unmakeMove() {
    if (moveHistoryStack.empty())
        return; 
    
    MoveHistory history = this->moveHistoryStack.back();
    this->moveHistoryStack.pop_back();
    Move move = history.move;
    
    sideToMove = (sideToMove == Piece::White) ? Piece::Black : Piece::White;
    

    int movedPiece = board[move.to];
    if (move.isPromotion)
        movedPiece = (movedPiece & (Piece::White | Piece::Black)) | Piece::Pawn;
    
    board[move.from] = movedPiece;
    board[move.to] = history.capturedPiece;
    
    if (move.isCastling) {
        board[history.rookFrom] = history.rookPiece;
        board[history.rookTo] = Piece::None;
    }
    
    if (move.isEnPassant) {
        int capturedPawnSquare = move.to + ((sideToMove == Piece::White) ? 8 : -8);
        board[capturedPawnSquare] = move.capturePiece;
    }
    
    enPassantTarget = history.oldEnPassant;
    
    canCastleKingsideWhite = history.oldCanCastleKingsideWhite;
    canCastleQueensideWhite = history.oldCanCastleQueensideWhite;
    canCastleKingsideBlack = history.oldCanCastleKingsideBlack;
    canCastleQueensideBlack = history.oldCanCastleQueensideBlack;

    moveCount--;
}
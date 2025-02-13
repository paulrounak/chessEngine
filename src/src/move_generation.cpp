#include "../headers/board.h"

std::vector<Move> Board::generatePawnMoves(int square, int color) {
    std::vector<Move> moves;
    int direction = (color == Piece::White) ? -8 : 8;
    int forward = square + direction;
    bool isPromotionRank = (color == Piece::White) ? (forward < 8) : (forward >= 56);

    // Forward moves
    if (forward >= 0 && forward < 64 && board[forward] == Piece::None) {
        if (isPromotionRank) {
            for (int promo : {Piece::Queen, Piece::Rook, Piece::Bishop, Piece::Knight})
                moves.push_back(Move(square, forward, Piece::None, true, false, promo, false));
        } else {
            moves.push_back(Move(square, forward, Piece::None, false, false, Piece::None, false));
            int startRank = (color == Piece::White) ? 6 : 1;
            if (square / 8 == startRank) {
                int doubleForward = forward + direction;
                if (doubleForward >= 0 && doubleForward < 64 && board[doubleForward] == Piece::None)
                    moves.push_back(Move(square, doubleForward, Piece::None, false, false, Piece::None, false));
            }
        }
    }

    // Captures
    std::vector<int> captureOffsets = {direction - 1, direction + 1};
    for (int offset : captureOffsets) {
        int target = square + offset;
        if (target < 0 || target >= 64) continue;
        int targetFile = target % 8;
        int currFile = square % 8;
        if (abs(targetFile - currFile) != 1) continue;

        bool isEp = (target == enPassantTarget);
        if (isEp) {
            int requiredPawn = (color == Piece::White) ? (Piece::Pawn | Piece::Black) : (Piece::Pawn | Piece::White);
            int pawnSquare = target - direction;
            if (board[pawnSquare] != requiredPawn)
                continue; // Skip if no pawn to capture
        }

        int capturedPiece = isEp ? (Piece::Pawn | (color == Piece::White ? Piece::Black : Piece::White)) : board[target];
        if (isEp || (board[target] != Piece::None && ((board[target] & (Piece::White | Piece::Black)) != color))) {
            if (isPromotionRank) {
                for (int promo : {Piece::Queen, Piece::Rook, Piece::Bishop, Piece::Knight})
                    moves.push_back(Move(square, target, capturedPiece, true, isEp, promo, false));
            } else {
                moves.push_back(Move(square, target, capturedPiece, false, isEp, Piece::None, false));
            }
        }
    }

    return moves;
}

std::vector<Move> Board::generateKnightMoves(int square, int color) {
    std::vector<Move> moves;
    std::vector<int> offsets = {-17, -15, -10, -6, 6, 10, 15, 17};
    for (int offset : offsets) {
        int target = square + offset;
        if (target < 0 || target >= 64) continue;
        int dx = abs((target % 8) - (square % 8));
        int dy = abs((target / 8) - (square / 8));
        // Valid knight moves are (1,2) or (2,1)
        if (!((dx == 1 && dy == 2) || (dx == 2 && dy == 1)))
            continue;
        if (board[target] == Piece::None || ((board[target] & (Piece::White | Piece::Black)) != color))
            moves.push_back(Move(square, target, board[target], false, false, Piece::None, false));
    }
    return moves;
}

std::vector<Move> Board::generateRookMoves(int square, int color) {
    std::vector<Move> moves;
    std::vector<int> dirs = {-8, 8, -1, 1};
    for (int dir : dirs) {
        for (int step = 1;; step++) {
            int target = square + dir * step;
            if (target < 0 || target >= 64)
                break;
            // Prevent horizontal wrap‐around.
            if ((dir == -1 || dir == 1) && (target / 8 != square / 8))
                break;
            if (board[target] != Piece::None) {
                if ((board[target] & (Piece::White | Piece::Black)) != color)
                    moves.push_back(Move(square, target, board[target], false, false, Piece::None, false));
                break;
            }
            moves.push_back(Move(square, target, Piece::None, false, false, Piece::None, false));
        }
    }
    return moves;
}

std::vector<Move> Board::generateBishopMoves(int square, int color) {
    std::vector<Move> moves;
    std::vector<int> dirs = {-9, -7, 7, 9};
    for (int dir : dirs) {
        for (int step = 1;; step++) {
            int target = square + dir * step;
            if (target < 0 || target >= 64)
                break;
            int dx = abs((target % 8) - (square % 8));
            int dy = abs((target / 8) - (square / 8));
            if (dx != dy)
                break;
            if (board[target] != Piece::None) {
                if ((board[target] & (Piece::White | Piece::Black)) != color)
                    moves.push_back(Move(square, target, board[target], false, false, Piece::None, false));
                break;
            }
            moves.push_back(Move(square, target, Piece::None, false, false, Piece::None, false));
        }
    }
    return moves;
}

std::vector<Move> Board::generateQueenMoves(int square, int color) {
    std::vector<Move> moves = generateRookMoves(square, color);
    std::vector<Move> bishopMoves = generateBishopMoves(square, color);
    moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
    return moves;
}

std::vector<Move> Board::generateKingMoves(int square, int color, bool canCastleK, bool canCastleQ) {
    std::vector<Move> moves;
    std::vector<int> dirs = {-9, -8, -7, -1, 1, 7, 8, 9};
    for (int dir : dirs) {
        int target = square + dir;
        if (target < 0 || target >= 64)
            continue;
        int dx = abs((target % 8) - (square % 8));
        int dy = abs((target / 8) - (square / 8));
        if (dx > 1 || dy > 1)
            continue;
        if (board[target] == Piece::None || ((board[target] & (Piece::White | Piece::Black)) != color))
            moves.push_back(Move(square, target, board[target], false, false, Piece::None, false));
    }

    int rank = (color == Piece::White) ? 7 : 0;
    int opponent = (color == Piece::White) ? Piece::Black : Piece::White;
    bool kingInCheck = isKingInCheck(color);

    // Kingside castling: king moves two squares right.
    if (canCastleK && !kingInCheck) {
        int rookSquare = rank * 8 + 7;
        if ((board[rookSquare] & (color | Piece::Rook)) == (color | Piece::Rook)) {
            if (board[rank*8+5] == Piece::None && board[rank*8+6] == Piece::None &&
                !isSquareAttacked(rank*8+5, opponent) &&
                !isSquareAttacked(rank*8+6, opponent))
            {
                moves.push_back(Move(square, rank*8+6, Piece::None, false, false, Piece::None, true));
            }
        }
    }

    // Queenside castling: king moves two squares left.
    if (canCastleQ && !kingInCheck) {
        int rookSquare = rank * 8;
        if ((board[rookSquare] & (color | Piece::Rook)) == (color | Piece::Rook)) {
            if (board[rank*8+1] == Piece::None && board[rank*8+2] == Piece::None && board[rank*8+3] == Piece::None &&
                !isSquareAttacked(rank*8+2, opponent) &&
                !isSquareAttacked(rank*8+3, opponent))
            {
                moves.push_back(Move(square, rank*8+2, Piece::None, false, false, Piece::None, true));
            }
        }
    }

    return moves;
}

#include "../headers/board.h"

bool Board::isSquareAttacked(int square, int attackerColor) {

    // Pawn attacks
    std::vector<int> pawnAttackSquares;
    if (attackerColor == Piece::White) {
        pawnAttackSquares = {square + 7, square + 9};
    } else {
        pawnAttackSquares = {square - 7, square - 9};
    }
    for (int attackerSquare : pawnAttackSquares) {
        if (attackerSquare < 0 || attackerSquare >= 64)
            continue;
        if (abs((attackerSquare % 8) - (square % 8)) != 1)
            continue;
        int piece = board[attackerSquare];
        if (piece != Piece::None &&
            (piece & (Piece::White | Piece::Black)) == attackerColor &&
            ((piece & 7) == Piece::Pawn))
            return true;
    }

    // Knight attacks
    std::vector<int> knightOffsets = {-17, -15, -10, -6, 6, 10, 15, 17};
    for (int offset : knightOffsets) {
        int target = square + offset;
        if (target >= 0 && target < 64) {
            int dx = abs((target % 8) - (square % 8));
            int dy = abs((target / 8) - (square / 8));
            if ((dx == 1 && dy == 2) || (dx == 2 && dy == 1)) {
                int targetPiece = board[target];
                if (targetPiece != Piece::None &&
                    (targetPiece & attackerColor) &&
                    ((targetPiece & 7) == Piece::Knight))
                    return true;
            }
        }
    }

    // Rook and Queen attacks (horizontal and vertical)
    std::vector<int> rookDirs = {-8, 8, -1, 1};
    for (int dir : rookDirs) {
        for (int step = 1;; step++) {
            int target = square + dir * step;
            if (target < 0 || target >= 64)
                break;
            if ((dir == -1 || dir == 1) && (target / 8 != square / 8))
                break;
            int targetPiece = board[target];
            if (targetPiece != Piece::None) {
                if ((targetPiece & attackerColor) &&
                    (((targetPiece & 7) == Piece::Rook) || ((targetPiece & 7) == Piece::Queen)))
                    return true;
                break;
            }
        }
    }

    // Bishop and Queen attacks (diagonals)
    std::vector<int> bishopDirs = {-9, -7, 7, 9};
    for (int dir : bishopDirs) {
        for (int step = 1;; step++) {
            int target = square + dir * step;
            if (target < 0 || target >= 64)
                break;
            int dx = abs((target % 8) - (square % 8));
            int dy = abs((target / 8) - (square / 8));
            if (dx != dy)
                break;
            int targetPiece = board[target];
            if (targetPiece != Piece::None) {
                if ((targetPiece & attackerColor) &&
                    (((targetPiece & 7) == Piece::Bishop) || ((targetPiece & 7) == Piece::Queen)))
                    return true;
                break;
            }
        }
    }

    // King attacks
    std::vector<int> kingDirs = {-9, -8, -7, -1, 1, 7, 8, 9};
    for (int dir : kingDirs) {
        int target = square + dir;
        if (target >= 0 && target < 64) {
            int dx = abs((target % 8) - (square % 8));
            int dy = abs((target / 8) - (square / 8));
            if (dx <= 1 && dy <= 1) {
                int targetPiece = board[target];
                if (targetPiece != Piece::None &&
                    (targetPiece & attackerColor) &&
                    ((targetPiece & 7) == Piece::King))
                    return true;
            }
        }
    }

    return false;
}

// Find the king of a given color.
bool Board::isKingInCheck(int color) {
    int kingPos = -1;
    for (int i = 0; i < 64; i++) {
        if ((board[i] & 7) == Piece::King && (board[i] & (Piece::White | Piece::Black)) == color) {
            kingPos = i;
            break;
        }
    }

    if (kingPos == -1) return false;
    int enemyColor = (color == Piece::White) ? Piece::Black : Piece::White;
    return isSquareAttacked(kingPos, enemyColor);
}
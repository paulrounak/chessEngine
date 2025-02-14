#include "../headers/utils.h"
#include "../headers/board.h"

Piece Board::getPieceAt(int square) const {
    int pieceCode = board[square];
    Piece::Type type = static_cast<Piece::Type>(pieceCode & 0x7);
    Piece::Color color = static_cast<Piece::Color>(pieceCode & 0x18);
    return Piece(type, color);
}


std::string squareToString(int square) {
    int file = square % 8;
    int rank = square / 8;
    char fileChar = 'a' + file;
    char rankChar = '8' - rank;
    return std::string() + fileChar + rankChar;
}

std::string moveToUCI(const Move& move) {
    std::string moveStr = squareToString(move.from) + squareToString(move.to);
    if (move.isPromotion) {
        char promoChar;
        switch (move.promotionPiece) {
            case Piece::Queen:  promoChar = 'q'; break;
            case Piece::Rook:   promoChar = 'r'; break;
            case Piece::Bishop: promoChar = 'b'; break;
            case Piece::Knight: promoChar = 'n'; break;
            default:            promoChar = 'q';
        }
        moveStr += promoChar;
    }
    return moveStr;
}
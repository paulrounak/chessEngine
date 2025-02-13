#include "../headers/utils.h"

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
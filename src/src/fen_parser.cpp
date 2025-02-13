#include "../headers/board.h"

void Board::fenPosition(const std::string& fen) {
    std::unordered_map<char, int> pieceTable = {
        {'k', Piece::King}, {'q', Piece::Queen}, {'p', Piece::Pawn},
        {'b', Piece::Bishop}, {'n', Piece::Knight}, {'r', Piece::Rook}
    };

    fill(board.begin(), board.end(), Piece::None);
    std::stringstream ss(fen);
    std::string boardPart, turn, castling, enPassant;
    int halfmoveClock, fullmoveNumber;
    ss >> boardPart >> turn >> castling >> enPassant >> halfmoveClock >> fullmoveNumber;

    int rank = 0, file = 0;
    for (char c : boardPart) {
        if (c == '/') {
            rank++;
            file = 0;
        } else if (isdigit(c)) {
            file += c - '0';
        } else {
            int color = isupper(c) ? Piece::White : Piece::Black;
            int piece = pieceTable[tolower(c)];
            board[rank * 8 + file] = piece | color;
            file++;
        }
    }

    sideToMove = (turn == "w") ? Piece::White : Piece::Black;
    canCastleKingsideWhite = (castling.find('K') != std::string::npos);
    canCastleQueensideWhite = (castling.find('Q') != std::string::npos);
    canCastleKingsideBlack = (castling.find('k') != std::string::npos);
    canCastleQueensideBlack = (castling.find('q') != std::string::npos);

    enPassantTarget = -1;
    if (enPassant != "-") {
        int fileEp = enPassant[0] - 'a';
        int rankEp = 8 - (enPassant[1] - '0');
        enPassantTarget = rankEp * 8 + fileEp;
    }
}

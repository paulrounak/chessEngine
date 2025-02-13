#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <cstdlib>
#include <random>
#include <cctype>
#include <cmath>
#include <algorithm>

using namespace std;

/*
+----+----+----+----+----+----+----+----+
|  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |   black
+----+----+----+----+----+----+----+----+
|  8 |  9 | 10 | 11 | 12 | 13 | 14 | 15 |
+----+----+----+----+----+----+----+----+
| 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 |
+----+----+----+----+----+----+----+----+
| 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
+----+----+----+----+----+----+----+----+
| 32 | 33 | 34 | 35 | 36 | 37 | 38 | 39 |
+----+----+----+----+----+----+----+----+
| 40 | 41 | 42 | 43 | 44 | 45 | 46 | 47 |
+----+----+----+----+----+----+----+----+
| 48 | 49 | 50 | 51 | 52 | 53 | 54 | 55 |
+----+----+----+----+----+----+----+----+
| 56 | 57 | 58 | 59 | 60 | 61 | 62 | 63 |   white
+----+----+----+----+----+----+----+----+
*/

class Piece {
public:
    enum {
        None   = 0,
        King   = 1,
        Pawn   = 2,
        Knight = 3,
        Bishop = 4,
        Rook   = 5,
        Queen  = 6,
        White  = 8,
        Black  = 16
    };
};

struct Move {
    int from;
    int to;
    int capturePiece;
    bool isPromotion;
    bool isEnPassant;
    int promotionPiece;
    bool isCastling;

    Move(int f, int t, int cap, bool promo, bool ep, int promoPiece, bool castle)
        : from(f), to(t), capturePiece(cap), isPromotion(promo), isEnPassant(ep),
          promotionPiece(promoPiece), isCastling(castle) {}

    Move() : from(-1), to(-1), capturePiece(Piece::None), isPromotion(false),
             isEnPassant(false), promotionPiece(Piece::None), isCastling(false) {}
};

class Board {
public:
    vector<int> board;
    int sideToMove;
    bool canCastleKingsideWhite, canCastleQueensideWhite;
    bool canCastleKingsideBlack, canCastleQueensideBlack;
    int enPassantTarget;

    Board() : board(64, Piece::None), sideToMove(Piece::White),
              canCastleKingsideWhite(false), canCastleQueensideWhite(false),
              canCastleKingsideBlack(false), canCastleQueensideBlack(false),
              enPassantTarget(-1) {}

    void fenPosition(const string& fen) {
        unordered_map<char, int> pieceTable = {
            {'k', Piece::King}, {'q', Piece::Queen}, {'p', Piece::Pawn},
            {'b', Piece::Bishop}, {'n', Piece::Knight}, {'r', Piece::Rook}
        };

        fill(board.begin(), board.end(), Piece::None);
        stringstream ss(fen);
        string boardPart, turn, castling, enPassant;
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
        canCastleKingsideWhite = (castling.find('K') != string::npos);
        canCastleQueensideWhite = (castling.find('Q') != string::npos);
        canCastleKingsideBlack = (castling.find('k') != string::npos);
        canCastleQueensideBlack = (castling.find('q') != string::npos);

        enPassantTarget = -1;
        if (enPassant != "-") {
            int fileEp = enPassant[0] - 'a';
            int rankEp = 8 - (enPassant[1] - '0');
            enPassantTarget = rankEp * 8 + fileEp;
        }
    }

    vector<Move> generatePawnMoves(int square, int color) {
        vector<Move> moves;
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
        vector<int> captureOffsets = {direction - 1, direction + 1};
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

    vector<Move> generateKnightMoves(int square, int color) {
        vector<Move> moves;
        vector<int> offsets = {-17, -15, -10, -6, 6, 10, 15, 17};
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

    vector<Move> generateRookMoves(int square, int color) {
        vector<Move> moves;
        vector<int> dirs = {-8, 8, -1, 1};
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

    vector<Move> generateBishopMoves(int square, int color) {
        vector<Move> moves;
        vector<int> dirs = {-9, -7, 7, 9};
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

    vector<Move> generateQueenMoves(int square, int color) {
        vector<Move> moves = generateRookMoves(square, color);
        vector<Move> bishopMoves = generateBishopMoves(square, color);
        moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
        return moves;
    }

    vector<Move> generateKingMoves(int square, int color, bool canCastleK, bool canCastleQ) {
        vector<Move> moves;
        vector<int> dirs = {-9, -8, -7, -1, 1, 7, 8, 9};
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

    bool isSquareAttacked(int square, int attackerColor) {

        // Pawn attacks
        vector<int> pawnAttackSquares;
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
        vector<int> knightOffsets = {-17, -15, -10, -6, 6, 10, 15, 17};
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
        vector<int> rookDirs = {-8, 8, -1, 1};
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
        vector<int> bishopDirs = {-9, -7, 7, 9};
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
        vector<int> kingDirs = {-9, -8, -7, -1, 1, 7, 8, 9};
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
    bool isKingInCheck(int color) {
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

    // Generate legal moves by filtering out pseudo–moves that leave the king in check.
    vector<Move> generateLegalMoves() {
        vector<Move> legalMoves;
        vector<Move> pseudoMoves;

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

    // Make a move – this function also updates castling rights and en passant targets.
    void makeMove(const Move& move) {
        int fromPiece = board[move.from];
        int color = fromPiece & (Piece::White | Piece::Black);
        int pieceType = fromPiece & 7;

        // Update castling rights if the king moves.
        if (pieceType == Piece::King) {
            if (color == Piece::White) {
                canCastleKingsideWhite = false;
                canCastleQueensideWhite = false;
            } else {
                canCastleKingsideBlack = false;
                canCastleQueensideBlack = false;
            }
        }
        // If a rook moves from its starting square, cancel its castling right.
        else if (pieceType == Piece::Rook) {
            if (color == Piece::White) {
                if (move.from == 7 * 8 + 7) { // h1
                    canCastleKingsideWhite = false;
                } else if (move.from == 7 * 8 + 0) { // a1
                    canCastleQueensideWhite = false;
                }
            } else {
                if (move.from == 0 * 8 + 7) { // h8
                    canCastleKingsideBlack = false;
                } else if (move.from == 0 * 8 + 0) { // a8
                    canCastleQueensideBlack = false;
                }
            }
        }
        
        // If a rook is captured from its starting square, update the opponent's castling rights.
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

        // Handle castling.
        if (move.isCastling) {
            int rookFrom, rookTo;
            if (move.to == move.from + 2) { // kingside
                rookFrom = move.from + 3;
                rookTo = move.from + 1;
            } else { // queenside
                rookFrom = move.from - 4;
                rookTo = move.from - 1;
            }
            board[rookTo] = board[rookFrom];
            board[rookFrom] = Piece::None;
        }

        // Handle en passant.
        if (move.isEnPassant) {
            int capturedPawnSquare = move.to + ((color == Piece::White) ? 8 : -8);
            board[capturedPawnSquare] = Piece::None;
            enPassantTarget = -1;
        } else if (pieceType == Piece::Pawn && abs(move.to - move.from) == 16) {
            // Set en passant target if a pawn moved two squares.
            enPassantTarget = move.from + (move.to - move.from) / 2;
        } else {
            enPassantTarget = -1;
        }

        // Handle promotion.
        if (move.isPromotion) {
            fromPiece = color | move.promotionPiece;
        }

        // Perform the move.
        board[move.from] = Piece::None;
        board[move.to] = fromPiece;

        // Toggle side to move.
        sideToMove = (sideToMove == Piece::White) ? Piece::Black : Piece::White;
    }
};

string squareToString(int square) {
    int file = square % 8;
    int rank = square / 8;
    char fileChar = 'a' + file;
    char rankChar = '8' - rank;
    return string() + fileChar + rankChar;
}

string moveToUCI(const Move& move) {
    string moveStr = squareToString(move.from) + squareToString(move.to);
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

int main() {
    Board board;
    string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board.fenPosition(startFEN);

    string line;
    while (getline(cin, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string token;
        iss >> token;

        if (token == "uci") {
            cout << "id name chessEngine\n";
            cout << "id author Rounak Paul\n";
            cout << "uciok\n";
        } else if (token == "isready") {
            cout << "readyok\n";
        } else if (token == "position") {
            string posType;
            iss >> posType;
            if (posType == "startpos") {
                board.fenPosition(startFEN);
                string movesToken;
                if (iss >> movesToken && movesToken == "moves") {
                    string moveStr;
                    while (iss >> moveStr) {
                        vector<Move> legalMoves = board.generateLegalMoves();
                        for (const Move& m : legalMoves) {
                            if (moveToUCI(m) == moveStr) {
                                board.makeMove(m);
                                break;
                            }
                        }
                    }
                }
            } else if (posType == "fen") {
                string fen, tokenPart;
                // Read the FEN until we hit "moves" (if present).
                while (iss >> tokenPart && tokenPart != "moves") {
                    fen += tokenPart + " ";
                }
                board.fenPosition(fen);
                if (tokenPart == "moves") {
                    string moveStr;
                    while (iss >> moveStr) {
                        vector<Move> legalMoves = board.generateLegalMoves();
                        for (const Move& m : legalMoves) {
                            if (moveToUCI(m) == moveStr) {
                                board.makeMove(m);
                                break;
                            }
                        }
                    }
                }
            }
        } else if (token == "go") {
            vector<Move> legalMoves = board.generateLegalMoves();
            if (!legalMoves.empty()) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, legalMoves.size() - 1);
                Move chosenMove = legalMoves[dis(gen)];
                cout << "bestmove " << moveToUCI(chosenMove) << "\n";
            } else {
                cout << "bestmove 0000\n";
            }
        } else if (token == "quit") {
            break;
        }
    }
    return 0;
}

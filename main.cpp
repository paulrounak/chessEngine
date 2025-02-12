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

    // Set board from a FEN string.
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

        // FEN ranks are separated by '/'
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
            int rankEp = 8 - (enPassant[1] - '0'); // rank0 is the 8th rank
            enPassantTarget = rankEp * 8 + fileEp;
        }
    }

    vector<Move> generatePawnMoves(int square, int color) {
        vector<Move> moves;
        int direction = (color == Piece::White) ? -8 : 8;
        int forward = square + direction;
        bool isPromotionRank = (color == Piece::White) ? (forward < 8) : (forward >= 56);

        // Single forward move
        if (forward >= 0 && forward < 64 && board[forward] == Piece::None) {
            if (isPromotionRank) {
                for (int promo : {Piece::Queen, Piece::Rook, Piece::Bishop, Piece::Knight})
                    moves.push_back(Move(square, forward, Piece::None, true, false, promo, false));
            } else {
                moves.push_back(Move(square, forward, Piece::None, false, false, Piece::None, false));
                // Double move from starting rank
                int startRank = (color == Piece::White) ? 6 : 1;
                if (square / 8 == startRank) {
                    int doubleForward = forward + direction;
                    if (doubleForward >= 0 && doubleForward < 64 && board[doubleForward] == Piece::None)
                        moves.push_back(Move(square, doubleForward, Piece::None, false, false, Piece::None, false));
                }
            }
        }

        // Captures and en passant
        vector<int> captureOffsets = {direction - 1, direction + 1};
        for (int offset : captureOffsets) {
            int target = square + offset;
            if (target < 0 || target >= 64) continue;
            int targetFile = target % 8;
            int currFile = square % 8;
            if (abs(targetFile - currFile) != 1) continue; // ensure diagonal

            bool isEp = (target == enPassantTarget);
            int capturedPawnSquare = isEp ? (target - direction) : target;
            int capturedPiece = isEp ? (Piece::Pawn | (color == Piece::White ? Piece::Black : Piece::White)) : board[target];

            if (isEp) {
                int requiredRank = (color == Piece::White) ? 3 : 4;
                if (square / 8 != requiredRank)
                    continue;
            }

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
            if (dx + dy != 3 || dx == 0 || dy == 0) continue;
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

        // Castling (only if king is on its original square)
        int rank = (color == Piece::White) ? 7 : 0;
        if (square == rank * 8 + 4) {
            // Kingside castling
            if (canCastleK &&
                board[rank*8+5] == Piece::None && board[rank*8+6] == Piece::None &&
                !isSquareAttacked(rank*8+4, (color == Piece::White) ? Piece::Black : Piece::White) &&
                !isSquareAttacked(rank*8+5, (color == Piece::White) ? Piece::Black : Piece::White) &&
                !isSquareAttacked(rank*8+6, (color == Piece::White) ? Piece::Black : Piece::White))
            {
                moves.push_back(Move(square, rank*8+6, Piece::None, false, false, Piece::None, true));
            }
            // Queenside castling
            if (canCastleQ &&
                board[rank*8+3] == Piece::None && board[rank*8+2] == Piece::None && board[rank*8+1] == Piece::None &&
                !isSquareAttacked(rank*8+4, (color == Piece::White) ? Piece::Black : Piece::White) &&
                !isSquareAttacked(rank*8+3, (color == Piece::White) ? Piece::Black : Piece::White) &&
                !isSquareAttacked(rank*8+2, (color == Piece::White) ? Piece::Black : Piece::White))
            {
                moves.push_back(Move(square, rank*8+2, Piece::None, false, false, Piece::None, true));
            }
        }
        return moves;
    }

    bool isSquareAttacked(int square, int attackerColor) {
        // Pawn attacks
        int pawnDir = (attackerColor == Piece::White) ? -8 : 8;
        vector<int> pawnCaptures = {square + pawnDir + 1, square + pawnDir - 1};
        for (int cap : pawnCaptures) {
            if (cap >= 0 && cap < 64) {
                int targetPiece = board[cap];
                if (targetPiece != Piece::None &&
                    (targetPiece & attackerColor) &&
                    ((targetPiece & 7) == Piece::Pawn))
                    return true;
            }
        }
        
        // Knight attacks
        vector<int> knightOffsets = {-17, -15, -10, -6, 6, 10, 15, 17};
        for (int offset : knightOffsets) {
            int target = square + offset;
            if (target >= 0 && target < 64) {
                int targetPiece = board[target];
                if (targetPiece != Piece::None &&
                    (targetPiece & attackerColor) &&
                    ((targetPiece & 7) == Piece::Knight))
                    return true;
            }
        }
        
        // Rook and Queen sliding moves (horizontal and vertical)
        vector<int> rookDirs = {-8, 8, -1, 1};
        for (int dir : rookDirs) {
            for (int step = 1;; step++) {
                int target = square + dir * step;
                if (target < 0 || target >= 64)
                    break;
                // For horizontal moves, ensure we remain on the same rank.
                if ((dir == -1 || dir == 1) && (target / 8 != square / 8))
                    break;
                int targetPiece = board[target];
                if (targetPiece != Piece::None) {
                    int type = targetPiece & 7;
                    if ((type == Piece::Rook || type == Piece::Queen) && (targetPiece & attackerColor))
                        return true;
                    break;
                }
            }
        }
        
        // Bishop and Queen sliding moves (diagonals)
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
                    int type = targetPiece & 7;
                    if ((type == Piece::Bishop || type == Piece::Queen) && (targetPiece & attackerColor))
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
                int targetPiece = board[target];
                if (targetPiece != Piece::None &&
                    (targetPiece & attackerColor) &&
                    ((targetPiece & 7) == Piece::King))
                    return true;
            }
        }
        
        return false;
    }

    bool isKingInCheck(int color) {
        int kingPos = -1;
        for (int i = 0; i < 64; i++) {
            if ((board[i] & (Piece::King | Piece::White | Piece::Black)) == (Piece::King | color)) {
                kingPos = i;
                break;
            }
        }
        if (kingPos == -1) return false;
        int opponent = (color == Piece::White) ? Piece::Black : Piece::White;
        return isSquareAttacked(kingPos, opponent);
    }

    vector<Move> generateLegalMoves() {
        vector<Move> legalMoves;
        vector<Move> pseudoMoves;

        for (int i = 0; i < 64; i++) {
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
                    break;
            }
            for (Move move : pseudoMoves) {
                // Backup state
                int fromPiece = board[move.from];
                int toPiece = board[move.to];
                int oldEnPassant = enPassantTarget;
                bool isEp = move.isEnPassant;
                int epPawnSquare = -1;
                bool isCastle = move.isCastling;
                int rookFrom = -1, rookTo = -1;

                // Make move
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
                    board[rookTo] = board[rookFrom];
                    board[rookFrom] = Piece::None;
                }

                bool inCheck = isKingInCheck(sideToMove);

                // Restore state
                board[move.from] = fromPiece;
                board[move.to] = toPiece;
                if (isEp)
                    board[epPawnSquare] = move.capturePiece;
                if (isCastle) {
                    board[rookFrom] = board[rookTo];
                    board[rookTo] = Piece::None;
                }
                enPassantTarget = oldEnPassant;

                if (!inCheck)
                    legalMoves.push_back(move);
            }
        }
        return legalMoves;
    }

    // A very basic makeMove function (without full state updates).
    void makeMove(const Move& move) {
        int fromPiece = board[move.from];
        board[move.from] = Piece::None;
        board[move.to] = fromPiece;
        // Note: castling rights, en passant, promotions, etc., are not fully handled here.
    }
};

// Converts a board index (0-63) into a UCI square string (e.g., "e2").
string squareToString(int square) {
    int file = square % 8;
    int rank = square / 8;
    char fileChar = 'a' + file;
    char rankChar = '8' - rank; // rank0 corresponds to '8'
    string s;
    s.push_back(fileChar);
    s.push_back(rankChar);
    return s;
}

// Converts a Move into a UCI move string.
string moveToUCI(const Move& move) {
    string moveStr = squareToString(move.from) + squareToString(move.to);
    if (move.isPromotion) {
        char promoChar;
        switch (move.promotionPiece) {
            case Piece::Queen:  promoChar = 'q'; break;
            case Piece::Rook:   promoChar = 'r'; break;
            case Piece::Bishop: promoChar = 'b'; break;
            case Piece::Knight: promoChar = 'n'; break;
            default: promoChar = 'q';
        }
        moveStr.push_back(promoChar);
    }
    return moveStr;
}

int main() {
    Board board;
    // Standard starting position FEN.
    string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board.fenPosition(startFEN);

    string line;
    while(getline(cin, line)) {
        if(line == "") continue;
        istringstream iss(line);
        string token;
        iss >> token;
        if(token == "uci") {
            cout << "id name chessEngine" << endl;
            cout << "id author Rounak Paul" << endl;
            cout << "uciok" << endl;
        } else if(token == "isready") {
            cout << "readyok" << endl;
        } else if(token == "position") {
            // Handle either "startpos" or "fen" positions.
            string posType;
            iss >> posType;
            if(posType == "startpos") {
                board.fenPosition(startFEN);
                string movesToken;
                iss >> movesToken;
                if(movesToken == "moves") {
                    string moveStr;
                    while(iss >> moveStr) {
                        vector<Move> legalMoves = board.generateLegalMoves();
                        bool moveFound = false;
                        for (Move m : legalMoves) {
                            if(moveToUCI(m) == moveStr) {
                                board.makeMove(m);
                                board.sideToMove = (board.sideToMove == Piece::White) ? Piece::Black : Piece::White;
                                moveFound = true;
                                break;
                            }
                        }
                    }
                }
            } else if(posType == "fen") {
                string fenStr;
                string fenRest;
                getline(iss, fenRest);
                size_t start = fenRest.find_first_not_of(" ");
                if(start != string::npos)
                    fenStr = fenRest.substr(start);
                board.fenPosition(fenStr);
            }
        } else if(token == "go") {
            vector<Move> legalMoves = board.generateLegalMoves();
            if(!legalMoves.empty()) {
                random_device rd;
                mt19937 gen(rd());
                uniform_int_distribution<> dis(0, legalMoves.size()-1);
                Move chosenMove = legalMoves[dis(gen)];
                cout << "bestmove " << moveToUCI(chosenMove) << "" << endl;
                board.makeMove(chosenMove);
                board.sideToMove = (board.sideToMove == Piece::White) ? Piece::Black : Piece::White;
            } else {
                cout << "bestmove 0000" << endl;
            }
        } else if(token == "quit") {
            break;
        }
    }
    return 0;
}

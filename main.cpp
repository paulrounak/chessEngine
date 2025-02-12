#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <cstdlib>

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
        None = 0,
        King = 1,
        Pawn = 2,
        Knight = 3,
        Bishop = 4,
        Rook = 5,
        Queen = 6,
        White = 8,
        Black = 16
    };
};

struct Move{
    int from, to;
    int capturePiece;
    bool isPromotion;
    bool isEnPassant;
    int promotionPiece = Piece::None;
};

class Board {
    vector<int> board;
    int sideToMove;
    bool canCastleKingsideWhite, canCastleQueensideWhite;
    bool canCastleKingsideBlack, canCastleQueensideBlack;
    int enPassantTarget;

public:
    Board() : board(64, Piece::None), sideToMove(Piece::White),
              canCastleKingsideWhite(false), canCastleQueensideWhite(false),
              canCastleKingsideBlack(false), canCastleQueensideBlack(false),
              enPassantTarget(-1) {}

    void fenPosition(const string& fen) {
        unordered_map<char, int> pieceTable = {
            {'k', Piece::King},
            {'q', Piece::Queen},
            {'p', Piece::Pawn},
            {'b', Piece::Bishop},
            {'n', Piece::Knight},
            {'r', Piece::Rook}
        };

        fill(board.begin(), board.end(), Piece::None);
        stringstream ss(fen);
        string boardPart, turn, castling, enPassant;
        int halfmoveClock, fullmoveNumber;

        ss >> boardPart >> turn >> castling >> enPassant >> halfmoveClock >> fullmoveNumber;

        int rankIndex = 0, fileIndex = 0;
        for (char c : boardPart) {
            if (c == '/') {
                rankIndex++;
                fileIndex = 0;
            } else if (isdigit(c)) {
                fileIndex += c - '0';
            } else {
                int color = isupper(c) ? Piece::White : Piece::Black;
                int piece = pieceTable[tolower(c)];
                board[rankIndex * 8 + fileIndex] = piece | color;
                fileIndex++;
            }
        }

        sideToMove = (turn == "w") ? Piece::White : Piece::Black;

        canCastleKingsideWhite = castling.find('K') != string::npos;
        canCastleQueensideWhite = castling.find('Q') != string::npos;
        canCastleKingsideBlack = castling.find('k') != string::npos;
        canCastleQueensideBlack = castling.find('q') != string::npos;

        if (enPassant != "-") {
            int file = enPassant[0] - 'a';
            int rank = enPassant[1] - '1';
            enPassantTarget = rank * 8 + file;
        } else {
            enPassantTarget = -1;
        }
    }

    vector<Move> generatePawnMoves(int square, int color) {
        vector<Move> moves;
        int direction = (color == Piece::White) ? -8 : 8;
        int forward = square + direction;
        bool isPromotionRank = (color == Piece::White) ? (forward < 8) : (forward >= 56);

        // Normal Forward Move
        if (board[forward] == Piece::None) {
            if (isPromotionRank) {
                moves.push_back({square, forward, Piece::None, true, false, Piece::Queen});
                moves.push_back({square, forward, Piece::None, true, false, Piece::Rook});
                moves.push_back({square, forward, Piece::None, true, false, Piece::Bishop});
                moves.push_back({square, forward, Piece::None, true, false, Piece::Knight});
            } else {
                moves.push_back({square, forward, Piece::None, false, false, Piece::None});
            }

            // Double Push
            int startRank = (color == Piece::White) ? 48 : 8;
            if (square >= startRank && square < startRank + 8) {
                int doubleForward = forward + direction;
                if (board[doubleForward] == Piece::None) {
                    moves.push_back({square, doubleForward, Piece::None, false, false, Piece::None});
                }
            }
        }

        // Captures
        vector<int> captures = {forward - 1, forward + 1};
        for (int target : captures) {
            if (target >= 0 && target < 64) {
                bool isOpponentPiece = (board[target] != Piece::None) && 
                                    ((board[target] & Piece::White) != (color & Piece::White));
                if (isOpponentPiece) {
                    if (isPromotionRank) {
                        moves.push_back({square, target, board[target], true, false, Piece::Queen});
                        moves.push_back({square, target, board[target], true, false, Piece::Rook});
                        moves.push_back({square, target, board[target], true, false, Piece::Bishop});
                        moves.push_back({square, target, board[target], true, false, Piece::Knight});
                    } else {
                        moves.push_back({square, target, board[target], false, false, Piece::None});
                    }
                }
            }
        }

        return moves;
    }


    vector<Move> generateKnightMoves (int square, int color) {
        vector<Move> moves;
        vector<int> directions = { -17, -15, -10, -6, 6, 10, 15, 17 };

        int file = square % 8;

        for(int offset: directions){
            int target = square + offset;

            if (target >= 0 && target < 64) {
                int targetFile = target % 8;

                if ((abs(targetFile - file) == 1 && (abs(offset) == 15 || abs(offset) == 17)) ||
                    (abs(targetFile - file) == 2 && (abs(offset) == 6 || abs(offset) == 10))) {
                    
                    if (board[target] == Piece::None || (board[target] & Piece::White) != (color & Piece::White)) {
                        moves.push_back({square, target, board[target], false, false});
                    }
                }
            }
        }

        return moves;
    }

    vector<Move> generateRookMoves (int square, int color) {
        vector<Move> moves;
        vector<int> directions = {-8, 8, -1, 1};

        for (int offset: directions) {
            int target = square;

            while (true) {
                target += offset;

                if (target < 0 || target >= 64) 
                    break;

                if ((offset == -1 || offset == 1) && (target / 8 != square / 8))
                    break;

                if (board[target] == Piece::None) {
                    moves.push_back({square, target, Piece::None, false, false});
                } else {
                    if ((board[target] & Piece::White) != (color & Piece::White)) {
                        moves.push_back({square, target, board[target], false, false});
                    }
                    break;
                }
            }
        }

        return moves;
    }

    vector<Move> generateBishopMoves (int square, int color) {
        vector <Move> moves;
        vector<int> directions = { -9, -7, 7, 9 };

        for (int offset: directions) {
            int target = square;

            while(true) {
                target += offset;

                if(target < 0 || target >= 64)
                    break;

                int fileDiff = abs((target % 8) - (square % 8));
                int rankDiff = abs((target / 8) - (square / 8));
                if (fileDiff != rankDiff) 
                    break;

                if (board[target] == Piece::None) {
                    moves.push_back({square, target, Piece::None, false, false});
                } else {
                    if ((board[target] & Piece::White) != (color & Piece::White)) {
                        moves.push_back({square, target, board[target], false, false});
                    }
                    break;
                }
            }
        }

        return moves;
    }

    vector<Move> generateQueenMoves (int square, int color) {
        vector<Move> moves;
        vector<Move> rookMoves = generateRookMoves(square, color);
        vector<Move> bishopMoves = generateBishopMoves(square, color);

        moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
        moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());

        return moves;
    }

    vector<Move> generateKingMoves (int square, int color, bool canCastleKingside, bool canCastleQueenside) {
        vector<Move> moves;
        vector<int> directions = {-9, -8, -7, -1, 1, 7, 8, 9};

        for(int offset: directions) {
            int target = square + offset;

            if (target < 0 || target >= 64)
                continue;

            if ((offset == -1 || offset == 1) && (square / 8 != target / 8))
                continue;

            if (board[target] == Piece::None || (board[target] & Piece::White) != (color & Piece::White)) {
                moves.push_back({square, target, board[target], false, false});
            }
        }

        if (canCastleKingside || canCastleQueenside) {
            int rank = (color == Piece::White) ? 7 : 0; 
            int kingStart = rank * 8 + 4; 

            if (square == kingStart) {
                // Kingside Castling (O-O)
                if (canCastleKingside) {
                    int fSquare = kingStart + 1; 
                    int gSquare = kingStart + 2; 
                    int rookSquare = kingStart + 3;

                    if (board[fSquare] == Piece::None && board[gSquare] == Piece::None && 
                        board[rookSquare] == (Piece::Rook | color)) {
                        moves.push_back({kingStart, gSquare, Piece::None, false, false}); // King moves two squares
                    }
                }

                // Queenside Castling (O-O-O)
                if (canCastleQueenside) {
                    int dSquare = kingStart - 1; 
                    int cSquare = kingStart - 2; 
                    int bSquare = kingStart - 3; 
                    int rookSquare = kingStart - 4;

                    if (board[dSquare] == Piece::None && board[cSquare] == Piece::None &&
                        board[bSquare] == Piece::None && board[rookSquare] == (Piece::Rook | color)) {
                        moves.push_back({kingStart, cSquare, Piece::None, false, false}); // King moves two squares
                    }
                }
            }
        }

        return moves;
    }

    bool isSquareAttacked(int square, int attackerColor) {
        for (int i = 0; i < 64; i++) {
            if (board[i] != Piece::None && (board[i] & Piece::White) == attackerColor) {
                vector<Move> enemyMoves;
                
                if ((board[i] & 7) == Piece::Pawn)
                    enemyMoves = generatePawnMoves(i, attackerColor);
                else if ((board[i] & 7) == Piece::Knight)
                    enemyMoves = generateKnightMoves(i, attackerColor);
                else if ((board[i] & 7) == Piece::Bishop)
                    enemyMoves = generateBishopMoves(i, attackerColor);
                else if ((board[i] & 7) == Piece::Rook)
                    enemyMoves = generateRookMoves(i, attackerColor);
                else if ((board[i] & 7) == Piece::Queen)
                    enemyMoves = generateQueenMoves(i, attackerColor);
                else if ((board[i] & 7) == Piece::King)
                    enemyMoves = generateKingMoves(i, attackerColor, false, false);

                for (Move move : enemyMoves) {
                    if (move.to == square) return true;
                }
            }
        }
        return false;
    }

    bool isKingInCheck(int color) {
        int kingSquare = -1;
        for (int i = 0; i < 64; i++) {
            if ((board[i] & 7) == Piece::King && (board[i] & Piece::White) == color) {
                kingSquare = i;
                break;
            }
        }
        return isSquareAttacked(kingSquare, color == Piece::White ? Piece::Black : Piece::White);
    }

    vector<Move> generateLegalMoves() {
        vector<Move> legalMoves;
        
        for (int i = 0; i < 64; i++) {
            if (board[i] != Piece::None && (board[i] & Piece::White) == sideToMove) {
                vector<Move> pseudoLegalMoves;

                if ((board[i] & 7) == Piece::Pawn)
                    pseudoLegalMoves = generatePawnMoves(i, sideToMove);
                else if ((board[i] & 7) == Piece::Knight)
                    pseudoLegalMoves = generateKnightMoves(i, sideToMove);
                else if ((board[i] & 7) == Piece::Bishop)
                    pseudoLegalMoves = generateBishopMoves(i, sideToMove);
                else if ((board[i] & 7) == Piece::Rook)
                    pseudoLegalMoves = generateRookMoves(i, sideToMove);
                else if ((board[i] & 7) == Piece::Queen)
                    pseudoLegalMoves = generateQueenMoves(i, sideToMove);
                else if ((board[i] & 7) == Piece::King)
                    pseudoLegalMoves = generateKingMoves(i, sideToMove, canCastleKingsideWhite, canCastleQueensideWhite);

                for (Move move : pseudoLegalMoves) {
                    int capturedPiece = board[move.to];

                    // Make the move
                    board[move.to] = board[move.from];
                    board[move.from] = Piece::None;

                    bool stillInCheck = isKingInCheck(sideToMove);

                    // Undo the move
                    board[move.from] = board[move.to];
                    board[move.to] = capturedPiece;

                    if (!stillInCheck) {
                        legalMoves.push_back(move);
                    }
                }
            }
        }

        return legalMoves;
    }

};

void uci_loop() {
    string command;
    Board board;
    
    while (getline(cin, command)) {
        if (command == "uci") {
            cout << "id name chessEngine" << endl;
            cout << "id author Rounak Paul" << endl;
            cout << "uciok" << endl;
        } 
        else if (command == "isready") {
            cout << "readyok" << endl;
        } 
        else if (command.rfind("position", 0) == 0) {
            stringstream ss (command);
            string word;

            ss >> word;
            ss >> word;

            if (word == "fen") {
                string fen;
                ss >> fen;
                board.fenPosition(fen);
            } else if (word == "startpos"){
                string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
                board.fenPosition(fen);
            }

            cout << "Position set" << endl;
        }
        else if (command.rfind("go", 0) == 0) {
            vector<Move> legalMoves = board.generateLegalMoves();

            if (!legalMoves.empty()) {
                Move bestMove = legalMoves[rand() % legalMoves.size()];
                char fromFile = 'a' + (bestMove.from % 8);
                char fromRank = '1' + (7-(bestMove.from / 8));
                char toFile = 'a' + (bestMove.to % 8);
                char toRank = '1' + (7-(bestMove.to / 8));

                string moveStr = string() + fromFile + fromRank + toFile + toRank;
                if (bestMove.isPromotion) {
                    moveStr += "q"; // Always promote to queen for now
                }

                cout << "bestmove " << moveStr << endl;
            } else {
                cout << "bestmove 0000" << endl;
            }
        }
        else if (command == "quit") {
            break;
        }
    }
}

int main() {
    uci_loop();
    return 0;
}

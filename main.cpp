#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

using namespace std;

class Piece {
public:
    static const int None = 0;
    static const int King = 1;
    static const int Pawn = 2;
    static const int Knight = 3;
    static const int Bishop = 4;
    static const int Rook = 5;
    static const int Queen = 6;

    static const int White = 8;
    static const int Black = 16;
};

class Board {
    vector<int> board;

public:
    Board(): board(64, Piece::None) {};

    void fenPosition (const string& fen) {
        // rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

        unordered_map<char, int> pieceTable = {
            {'k', Piece::King},
            {'q', Piece::Queen},
            {'p', Piece::Pawn},
            {'b', Piece::Bishop},
            {'n', Piece::Knight},
            {'r', Piece::Rook}
        };

        fill (board.begin(), board.end(), Piece::None);

        stringstream ss(fen);
        string rank;
        int rankIndex = 0;

        while(getline(ss, rank, '/')){
            int fileIndex = 0;

            for(char c: rank){
                if (isdigit(c)) {
                    fileIndex += c - '0';
                } else {
                    int color = (isupper(c) ? Piece::White : Piece::Black);
                    int piece = pieceTable[tolower(c)];

                    board[rankIndex * 8 + fileIndex] = piece | color;
                    fileIndex++;
                }
            }

            rankIndex++;
        }
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
            // TODO: Implement move search logic
            cout << "bestmove h2h4" << endl;  // Dummy move
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

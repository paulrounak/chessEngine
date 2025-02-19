#include <iostream>
#include <string>
#include <sstream>
#include <random>
#include "../headers/board.h"
#include "../headers/utils.h"
#include "../headers/search.h"

int main() {
    Board board;
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    board.fenPosition(startFEN);

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string token;
        iss >> token;

        if (token == "uci") {
            std::cout << "id name chessEngine\n";
            std::cout << "id author Rounak Paul\n";
            std::cout << "uciok\n";
        } else if (token == "perft") {
            int depth;
            iss >> depth;
            for(int i = 1; i <= depth; i++)
                std::cout << "depth " << i << ": " << board.moveGenerationTest(i) << std::endl;
        } else if (token == "isready") {
            std::cout << "readyok\n";
        } else if (token == "position") {
            std::string posType;
            iss >> posType;
            if (posType == "startpos") {
                board.fenPosition(startFEN);
                std::string movesToken;
                if (iss >> movesToken && movesToken == "moves") {
                    std::string moveStr;
                    while (iss >> moveStr) {
                        std::vector<Move> legalMoves = board.generateLegalMoves();
                        for (const Move& m : legalMoves) {
                            if (moveToUCI(m) == moveStr) {
                                board.makeMove(m);
                                break;
                            }
                        }
                    }
                }
            } else if (posType == "fen") {
                std::string fen, tokenPart;
                while (iss >> tokenPart && tokenPart != "moves") {
                    fen += tokenPart + " ";
                }
                board.fenPosition(fen);
                if (tokenPart == "moves") {
                    std::string moveStr;
                    while (iss >> moveStr) {
                        std::vector<Move> legalMoves = board.generateLegalMoves();
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
            std::vector<Move> legalMoves = board.generateLegalMoves();
            Search search(board, 4);
            Move bestMove = search.findBestMove();

            std::cout << "bestmove " << moveToUCI(bestMove) << std::endl;

            int evaluation = Evaluator::evaluate(board);
            std::cout << "Evaluation: " << evaluation << std::endl;
        } else if (token == "quit") {
            break;
        }
    }
    return 0;
}
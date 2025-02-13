#include "../headers/board.h"

int Board::moveGenerationTest(int depth) {
    if (depth == 0)
        return 1;

    int nodes = 0;
    std::vector<Move> moves = generateLegalMoves();

    // Loop through each move.
    for (const Move &move : moves) {
        makeMove(move);

        nodes += moveGenerationTest(depth - 1);

        unmakeMove();
    }

    return nodes;
}
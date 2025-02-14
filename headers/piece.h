#ifndef PIECE_H
#define PIECE_H

class Piece {
public:
    enum Type {
        None   = 0,
        King   = 1,
        Pawn   = 2,
        Knight = 3,
        Bishop = 4,
        Rook   = 5,
        Queen  = 6
    };

    enum Color {
        White = 8,
        Black = 16
    };

    Piece(Type type = None, Color color = White) : type(type), color(color) {}

    Type getType() const { return type; }
    Color getColor() const { return color; }

private:
    Type type;
    Color color;
};

#endif

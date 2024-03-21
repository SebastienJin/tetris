#pragma once

#include <iostream>

#include "matrix.h"


enum piece_type
{
    NONE = 0,
    I,
    O,
    T,
    L,
    J,
    Z,
    S
};

enum Direction
{
    UP = 0,
    LEFT,
    DOWN,
    RIGHT
};

class Piece
{
public:
    piece_type type;
    unsigned int x;
    unsigned int y;
    Direction direction;

    Piece(Piece const &t): type(t.type), x(t.x), y(t.y), direction(t.direction) {}
    Piece(piece_type type_init): type(type_init), x(0), y(0) {}
    Piece(piece_type type_init, unsigned int x_init, unsigned int y_init): type(type_init), x(x_init), y(y_init) {}
    Piece(piece_type type_init, unsigned int x_init, unsigned int y_init, Direction direction_init): type(type_init), x(x_init), y(y_init), direction(direction_init) {}

    // check if the move is ok
    // false: if Piece outside the matrix or it collides with another Piece
    bool movement_test(Matrix<piece_type> &matrix)
    {
        Matrix<bool> piece_matrix = get_piece_matrix();
        for (unsigned int i = 0; i < piece_matrix.rows_value(); i++)
        {
            for (unsigned int j = 0; j < piece_matrix.cols_value(); j++)
            {
                if (piece_matrix.position_value(i, j) && (x + i >= matrix.rows_value() || y + j >= matrix.cols_value() || matrix.position_value(x + i, y + j) != NONE))
                {
                    return false;
                }
            }
        }
        return true;
    }

    // 4x4 matrix for "I"
    // 3x3 matrix for other types
    Matrix<bool> get_piece_matrix() const
    {
        Matrix<bool> piece_matrix((type != I) ? 3 : 4, (type != I) ? 3 : 4);

        switch (type)
        {
        case I:
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(1, 1, true);
            piece_matrix.change_value(2, 1, true);
            piece_matrix.change_value(3, 1, true);
            break;
        case O:
            piece_matrix.change_value(0, 0, true);
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(1, 0, true);
            piece_matrix.change_value(1, 1, true);
            break;
        case T:
            piece_matrix.change_value(0, 0, true);
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(0, 2, true);
            piece_matrix.change_value(1, 1, true);
            break;
        case L:
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(1, 1, true);
            piece_matrix.change_value(2, 1, true);
            piece_matrix.change_value(2, 2, true);
            break;
        case J:
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(1, 1, true);
            piece_matrix.change_value(2, 1, true);
            piece_matrix.change_value(2, 0, true);
            break;
        case Z:
            piece_matrix.change_value(0, 0, true);
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(1, 1, true);
            piece_matrix.change_value(1, 2, true);
            break;
        case S:
            piece_matrix.change_value(0, 1, true);
            piece_matrix.change_value(0, 2, true);
            piece_matrix.change_value(1, 0, true);
            piece_matrix.change_value(1, 1, true);
            break;
        default:
            std::cout << "Unknown piece_type" << std::endl;
            break;
        }

        // Piece O desn't rotate
        if (type != O)
        {
            switch (direction)
            {
            case UP:
                break;
            case RIGHT:
                piece_matrix.piece_matrix_rotation_right();
                break;
            case DOWN:
                piece_matrix.piece_matrix_rotation_left();
                piece_matrix.piece_matrix_rotation_left();
                break;
            case LEFT:
                piece_matrix.piece_matrix_rotation_left();
                break;
            default:
                std::cout << "Unknown Direction" << std::endl;
                break;
            }
        }

        return piece_matrix;
    }
};
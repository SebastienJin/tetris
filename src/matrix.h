#pragma once

#include <memory>
#include <algorithm>


template<class T>
class Matrix 
{
public:

    unsigned int rows;
    unsigned int cols;
    std::unique_ptr<T[]> data; 
    
    Matrix(const unsigned int rows_init, const unsigned int cols_init):
        rows(rows_init), cols(cols_init), data(std::make_unique<T[]>(rows_init * cols_init)) {}

    Matrix(const Matrix<T> &NewMatrix): 
        rows(NewMatrix.rows), cols(NewMatrix.cols) 
    {
        data = std::make_unique<T[]>(rows * cols);
        std::copy_n(NewMatrix.data.get(), NewMatrix.rows * NewMatrix.cols, data.get());
    }

    ~Matrix() {}

    unsigned int rows_value() const 
    {
        return rows;
    }

    unsigned int cols_value() const 
    {
        return cols;
    }
    
    T position_value(const int row, const int col) const 
    {
        return data[row * (cols) + col];
    }

    void change_value(const int row, const int col, const T value) 
    {
        data[row * (cols) + col] = value;
    }
    
    void change_all_value(const T value) 
    {
        for (int i = 0; i < rows_value(); i++) 
        {
            for (int j = 0; j < cols_value(); j++) 
            {
                change_value(i, j, value);
            }
        }
    }

    void piece_matrix_rotation_left() 
    {
        for (unsigned int i = 0; i < rows/2; i++) 
        {
            for (unsigned int j = i; j < cols-i-1; j++) 
            {
                int temp=position_value(i,j);
                change_value(i,j,position_value(j, rows-i-1));
                change_value(j,rows-i-1,position_value(rows-i-1,rows-j-1));
                change_value(rows-i-1,rows-j-1,position_value(rows-j-1,i));
                change_value(rows-j-1,i,temp);
            }
        }
    }

    void piece_matrix_rotation_right() 
    {
        for (unsigned int i = 0; i < rows/2; i++) 
        {
            for (unsigned int j = i; j < cols-i-1; j++) 
            {
                int temp=position_value(i,j);
                change_value(i,j,position_value(rows-j-1, i));
                change_value(rows-j-1, i, position_value(rows-i-1,rows-j-1));
                change_value(rows-i-1,rows-j-1,position_value(j, rows-i-1));
                change_value(j, rows-i-1,temp);
            }
        }
    }

};
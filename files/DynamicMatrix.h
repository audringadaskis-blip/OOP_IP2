#ifndef DYNAMIC_MATRIX_H
#define DYNAMIC_MATRIX_H

#include <string>
#include <exception>
#include <utility>

/**
 * @file DynamicMatrix.h
 * @brief Dynamic 2D integer matrix with CRUD/DAVE operations
 * @namespace AudrisMatrix
 * 
 * This class implements a dynamic matrix using the Pimpl idiom for
 * implementation hiding and RAII for resource management.
 * 
 * Features:
 * - Deep copy support (Rule of Three)
 * - Operator overloading for intuitive syntax
 * - Exception safety with custom MatrixException
 * - Const-correctness for read-only operations
 * 
 * @author Audris
 * @date March 2026
 * @version 1.2
 */

namespace AudrisMatrix {

    // custom exception class
    class MatrixException : public std::exception {
    private:
        std::string message;
    public:
        explicit MatrixException(const std::string& msg) : message(msg) {}
        const char* what() const noexcept override { return message.c_str(); }
    };

    class DynamicMatrix {
    private:
        // Pimpl pointer - vienintelis field .h faile
        struct Impl;
        Impl* pImpl;

    public:
        // --- constructor / destructor (RAII) ---
        DynamicMatrix();
        ~DynamicMatrix();

        // --- deep copy (Rule of Three) ---
        DynamicMatrix(const DynamicMatrix& other);
        DynamicMatrix& operator=(const DynamicMatrix& other);

        // --- operators (CRUD/DAVE) ---
        DynamicMatrix& operator+=(int value);
        DynamicMatrix& operator-=(int index);
        DynamicMatrix& operator*=(int factor);
        DynamicMatrix& operator!();
        std::pair<int, int> operator[](int value) const;

        // --- comparisons ---
        bool operator==(const DynamicMatrix& other) const;
        bool operator!=(const DynamicMatrix& other) const;
        bool operator<(const DynamicMatrix& other) const;
        bool operator<=(const DynamicMatrix& other) const;
        bool operator>(const DynamicMatrix& other) const;
        bool operator>=(const DynamicMatrix& other) const;

        // --- info ---
        std::string toString() const;

        // --- accessors ---
        int getRows() const;
        int getCols() const;
        int get(int row, int col) const;
        void set(int row, int col, int value);
    };

} // namespace AudrisMatrix

#endif // DYNAMIC_MATRIX_H
#include <sstream>
#include <stdexcept>
#include "DynamicMatrix.h"

namespace AudrisMatrix {

    // inner class (Pimpl) - matoma tik šiam failui
    struct DynamicMatrix::Impl {
        int** data;       // dinaminis 2D masyvas: pointer to pointer
        int rows;
        int cols;

        Impl() : data(nullptr), rows(0), cols(0) {}

        // sukuriamas tuščias masyvas nurodytiems matmenims
        static int** allocateMatrix(int r, int c) {
            if (r <= 0 || c <= 0) return nullptr;
            int** mat = new int*[r];
            for (int i = 0; i < r; ++i) {
                mat[i] = new int[c]();
            }
            return mat;
        }

        // išlaisvinama atmintis
        static void deallocateMatrix(int** mat, int r) {
            if (!mat) return;
            for (int i = 0; i < r; ++i) {
                delete[] mat[i];
            }
            delete[] mat;
        }

        // deep copy
        static int** copyMatrix(int** src, int r, int c) {
            if (!src || r <= 0 || c <= 0) return nullptr;
            int** dst = allocateMatrix(r, c);
            for (int i = 0; i < r; ++i) {
                for (int j = 0; j < c; ++j) {
                    dst[i][j] = src[i][j];
                }
            }
            return dst;
        }

        // copy constructor for Impl
        Impl(const Impl& other) : rows(other.rows), cols(other.cols) {
            data = copyMatrix(other.data, other.rows, other.cols);
        }

        // destructor for Impl
        ~Impl() {
            deallocateMatrix(data, rows);
        }
    };

    // --- constructor / destructor ---
    DynamicMatrix::DynamicMatrix() : pImpl(new Impl()) {}

    DynamicMatrix::~DynamicMatrix() {
        delete pImpl;  // Impl destruktorius išlaisvins data
    }

    // --- deep copy ---
    DynamicMatrix::DynamicMatrix(const DynamicMatrix& other)
        : pImpl(new Impl(*other.pImpl)) {}

    DynamicMatrix& DynamicMatrix::operator=(const DynamicMatrix& other) {
        if (this != &other) {
            Impl* newImpl = new Impl(*other.pImpl);
            delete pImpl;
            pImpl = newImpl;
        }
        return *this;
    }

    // --- operators ---

    // pridedama nauja eilė su 'value' reikšmėmis
    DynamicMatrix& DynamicMatrix::operator+=(int value) {
        if (pImpl->rows == 0) {
            pImpl->cols = 1;
            pImpl->data = Impl::allocateMatrix(1, 1);
            pImpl->data[0][0] = value;
            pImpl->rows = 1;
        } else {
            // sukuriamas naujas didesnis masyvas
            int** newData = Impl::allocateMatrix(pImpl->rows + 1, pImpl->cols);
            // kopijuojami seni duomenys
            for (int i = 0; i < pImpl->rows; ++i) {
                for (int j = 0; j < pImpl->cols; ++j) {
                    newData[i][j] = pImpl->data[i][j];
                }
            }
            // nauja eilutė užpildyta 'value'
            for (int j = 0; j < pImpl->cols; ++j) {
                newData[pImpl->rows][j] = value;
            }
            // išvaloma sena, pakeičiama nauju
            Impl::deallocateMatrix(pImpl->data, pImpl->rows);
            pImpl->data = newData;
            pImpl->rows++;
        }
        return *this;
    }

    // ištrinama eilė 'index'
    DynamicMatrix& DynamicMatrix::operator-=(int index) {
        if (index < 0 || index >= pImpl->rows) {
            throw MatrixException("Row index out of bounds");
        }
        if (pImpl->rows == 1) {
            // lieka tuščia matrica
            Impl::deallocateMatrix(pImpl->data, pImpl->rows);
            pImpl->data = nullptr;
            pImpl->rows = 0;
            pImpl->cols = 0;
        } else {
            int** newData = Impl::allocateMatrix(pImpl->rows - 1, pImpl->cols);
            int newIdx = 0;
            for (int i = 0; i < pImpl->rows; ++i) {
                if (i == index) continue;  // praleidžiama trintina eilutė
                for (int j = 0; j < pImpl->cols; ++j) {
                    newData[newIdx][j] = pImpl->data[i][j];
                }
                newIdx++;
            }
            Impl::deallocateMatrix(pImpl->data, pImpl->rows);
            pImpl->data = newData;
            pImpl->rows--;
        }
        return *this;
    }

    // visi elementai padauginami iš 'factor'
    DynamicMatrix& DynamicMatrix::operator*=(int factor) {
        if (!pImpl->data || pImpl->rows == 0 || pImpl->cols == 0) {
            return *this;  // tuščia matrica – nieko nedaroma
        }
        for (int i = 0; i < pImpl->rows; ++i) {
            for (int j = 0; j < pImpl->cols; ++j) {
                pImpl->data[i][j] *= factor;
            }
        }
        return *this;
    }

    // resetinama į tuščią
    DynamicMatrix& DynamicMatrix::operator!() {
        Impl::deallocateMatrix(pImpl->data, pImpl->rows);
        pImpl->data = nullptr;
        pImpl->rows = 0;
        pImpl->cols = 0;
        return *this;
    }

    // randama reikšmė ir grąžinama {row, col}
    std::pair<int, int> DynamicMatrix::operator[](int value) const {
        if (!pImpl->data || pImpl->rows == 0 || pImpl->cols == 0) {
            throw MatrixException("Value not found in matrix");
        }
        for (int i = 0; i < pImpl->rows; ++i) {
            for (int j = 0; j < pImpl->cols; ++j) {
                if (pImpl->data[i][j] == value) {
                    return {i, j};
                }
            }
        }
        throw MatrixException("Value not found in matrix");
    }

    // --- comparisons ---
    bool DynamicMatrix::operator==(const DynamicMatrix& other) const {
        if (pImpl->rows != other.pImpl->rows || pImpl->cols != other.pImpl->cols) {
            return false;
        }
        if (!pImpl->data && !other.pImpl->data) {
            return true;  // abi tuščios
        }
        if (!pImpl->data || !other.pImpl->data) {
            return false;  // viena tuščia, kita ne
        }
        for (int i = 0; i < pImpl->rows; ++i) {
            for (int j = 0; j < pImpl->cols; ++j) {
                if (pImpl->data[i][j] != other.pImpl->data[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool DynamicMatrix::operator!=(const DynamicMatrix& other) const {
        return !(*this == other);
    }

    bool DynamicMatrix::operator<(const DynamicMatrix& other) const {
        return (pImpl->rows * pImpl->cols) < (other.pImpl->rows * other.pImpl->cols);
    }

    bool DynamicMatrix::operator<=(const DynamicMatrix& other) const {
        return (*this < other) || (*this == other);
    }

    bool DynamicMatrix::operator>(const DynamicMatrix& other) const {
        return !(*this <= other);
    }

    bool DynamicMatrix::operator>=(const DynamicMatrix& other) const {
        return !(*this < other);
    }

    // --- info ---
    std::string DynamicMatrix::toString() const {
        std::stringstream ss;
        ss << "Matrix[" << pImpl->rows << "x" << pImpl->cols << "]:\n";
        if (!pImpl->data) {
            ss << "(empty)\n";
            return ss.str();
        }
        for (int i = 0; i < pImpl->rows; ++i) {
            for (int j = 0; j < pImpl->cols; ++j) {
                ss << pImpl->data[i][j] << " ";
            }
            ss << "\n";
        }
        return ss.str();
    }

    // --- accessors ---
    int DynamicMatrix::getRows() const { return pImpl->rows; }
    int DynamicMatrix::getCols() const { return pImpl->cols; }

    int DynamicMatrix::get(int row, int col) const {
        if (row < 0 || row >= pImpl->rows || col < 0 || col >= pImpl->cols) {
            throw MatrixException("Index out of bounds");
        }
        return pImpl->data[row][col];
    }

    void DynamicMatrix::set(int row, int col, int value) {
        if (row < 0 || row >= pImpl->rows || col < 0 || col >= pImpl->cols) {
            throw MatrixException("Index out of bounds");
        }
        pImpl->data[row][col] = value;
    }

} // namespace AudrisMatrix
#include "DynamicMatrix.h"
#include <iostream>

using namespace AudrisMatrix;

int main() {
    std::cout << "=== DynamicMatrix Demo ===" << std::endl;

    // 1. Konstruktorius ir tuščia matrica
    DynamicMatrix mat;
    std::cout << "1. Tuščia matrica:\n" << mat.toString() << std::endl;

    // 2. Insert: operator+=
    mat += 5;  // [5]
    mat += 3;  // [5], [3]
    mat += 7;  // [5], [3], [7]
    std::cout << "2. Po += operacijų:\n" << mat.toString() << std::endl;

    // 3. Edit: operator*=
    mat *= 2;  // visi elementai *2
    std::cout << "3. Po *= 2:\n" << mat.toString() << std::endl;

    // 4. Search: operator[]
    try {
        auto pos = mat[6];  // ieškome 6 (yra: 5*2=10, 3*2=6, 7*2=14)
        std::cout << "4. Rasta 6 pozicijoje: [" << pos.first << ", " << pos.second << "]" << std::endl;
    } catch (const MatrixException& e) {
        std::cout << "4. Klaida: " << e.what() << std::endl;
    }

    // 5. Accessors: get/set
    mat.set(0, 0, 100);
    std::cout << "5. Po set(0,0,100): " << mat.get(0, 0) << std::endl;

    // 6. Comparison: ==
    DynamicMatrix mat2;
    mat2 += 100;
    mat2 += 3;
    mat2 += 7;
    mat2 *= 2;
    mat2.set(0, 0, 100);
    std::cout << "6. mat == mat2: " << (mat == mat2 ? "true" : "false") << std::endl;

    // 7. Comparison: < (pagal elementų skaičių)
    DynamicMatrix small;
    small += 1;
    std::cout << "7. small < mat: " << (small < mat ? "true" : "false") << std::endl;

    // 8. Delete: operator-=
    mat -= 1;  // triname 1-ą eilutę (indeksas 1)
    std::cout << "8. Po -= 1:\n" << mat.toString() << std::endl;

    // 9. Clear: operator!
    !mat;
    std::cout << "9. Po ! (clear):\n" << mat.toString() << std::endl;

    // 10. Custom exception demo
    try {
        mat[999];  // ieškome neegzistuojančios reikšmės tuščioje matricoje
    } catch (const MatrixException& e) {
        std::cout << "10. Sugauta custom exception: " << e.what() << std::endl;
    }

    // 11. Bounds check exception
    try {
        mat.get(5, 0);  // neteisingas indeksas
    } catch (const MatrixException& e) {
        std::cout << "11. Sugauta bounds exception: " << e.what() << std::endl;
    }

    std::cout << "\n=== Demo completed ===" << std::endl;
    return 0;
}
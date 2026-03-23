#include "DynamicMatrix.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace AudrisMatrix;

// helper: logeris į stdout ir failą
class TestLogger {
private:
    std::ofstream logFile;
public:
    TestLogger(const std::string& filename) {
        logFile.open(filename);
    }
    ~TestLogger() {
        if (logFile.is_open()) logFile.close();
    }
    void log(const std::string& msg) {
        std::cout << msg << std::endl;
        if (logFile.is_open()) logFile << msg << std::endl;
    }
    bool is_open() const { return logFile.is_open(); }
};

// helper: testų rezultatas
void runTest(TestLogger& logger, const std::string& testName, bool condition) {
    std::string result = condition ? "PASS" : "FAIL";
    logger.log("[TEST] " + testName + ": " + result);
}

int main() {
    TestLogger logger("log.txt");
    if (!logger.is_open()) {
        std::cerr << "Error: cannot open log.txt" << std::endl;
        return 1;
    }

    logger.log("=== DynamicMatrix Test Suite ===");
    logger.log("Starting tests...\n");

    // Test 1: Default constructor
    {
        DynamicMatrix m;
        runTest(logger, "T1_default_ctor_empty", m.getRows() == 0 && m.getCols() == 0);
    }

    // Test 2: Insert operator+=
    {
        DynamicMatrix m;
        m += 42;
        runTest(logger, "T2_insert_one_row", m.getRows() == 1 && m.getCols() == 1 && m.get(0, 0) == 42);
    }

    // Test 3: Multiple inserts
    {
        DynamicMatrix m;
        m += 1; m += 2; m += 3;
        runTest(logger, "T3_multiple_inserts", m.getRows() == 3 && m.get(2, 0) == 3);
    }

    // Test 4: Edit operator*=
    {
        DynamicMatrix m;
        m += 5; m += 10;
        m *= 3;
        bool ok = (m.get(0, 0) == 15) && (m.get(1, 0) == 30);
        runTest(logger, "T4_edit_multiply", ok);
    }

    // Test 5: Search operator[] found
    {
        DynamicMatrix m;
        m += 7; m += 14; m += 21;
        try {
            auto pos = m[14];
            runTest(logger, "T5_search_found", pos.first == 1 && pos.second == 0);
        } catch (...) {
            runTest(logger, "T5_search_found", false);
        }
    }

    // Test 6: Search operator[] not found
    {
        DynamicMatrix m;
        m += 1; m += 2;
        bool caught = false;
        try {
            m[999];
        } catch (const MatrixException&) {
            caught = true;
        }
        runTest(logger, "T6_search_not_found_exception", caught);
    }

    // Test 7: Delete operator-= valid index
    {
        DynamicMatrix m;
        m += 1; m += 2; m += 3;
        m -= 1;  // remove middle
        bool ok = (m.getRows() == 2) && (m.get(0, 0) == 1) && (m.get(1, 0) == 3);
        runTest(logger, "T7_delete_valid_index", ok);
    }

    // Test 8: Delete operator-= invalid index
    {
        DynamicMatrix m;
        m += 1;
        bool caught = false;
        try {
            m -= 10;
        } catch (const MatrixException&) {
            caught = true;
        }
        runTest(logger, "T8_delete_invalid_index_exception", caught);
    }

    // Test 9: Clear operator!
    {
        DynamicMatrix m;
        m += 1; m += 2;
        !m;
        runTest(logger, "T9_clear_reset", m.getRows() == 0 && m.getCols() == 0);
    }

    // Test 10: Equality operator==
    {
        DynamicMatrix m1, m2;
        m1 += 5; m1 += 10;
        m2 += 5; m2 += 10;
        runTest(logger, "T10_equality_same", m1 == m2);
    }

    // Test 11: Inequality operator!=
    {
        DynamicMatrix m1, m2;
        m1 += 5;
        m2 += 7;
        runTest(logger, "T11_inequality_different", m1 != m2);
    }

    // Test 12: Less-than by element count
    {
        DynamicMatrix small, large;
        small += 1;
        large += 1; large += 2; large += 3;
        runTest(logger, "T12_less_by_count", small < large);
    }

    // Test 13: Deep copy constructor
    {
        DynamicMatrix m1;
        m1 += 100;
        DynamicMatrix m2(m1);
        m2.set(0, 0, 999);
        bool ok = (m1.get(0, 0) == 100) && (m2.get(0, 0) == 999);
        runTest(logger, "T13_deep_copy_independence", ok);
    }

    // Test 14: Assignment operator
    {
        DynamicMatrix m1, m2;
        m1 += 42;
        m2 = m1;
        m2 += 99;
        bool ok = (m1.getRows() == 1) && (m2.getRows() == 2);
        runTest(logger, "T14_assignment_independence", ok);
    }

    // Test 15: Accessor bounds check
    {
        DynamicMatrix m;
        m += 1;
        bool caught = false;
        try {
            m.get(10, 0);
        } catch (const MatrixException&) {
            caught = true;
        }
        runTest(logger, "T15_get_bounds_exception", caught);
    }

    // Test 16: toString format
    {
        DynamicMatrix m;
        m += 3; m += 7;
        std::string s = m.toString();
        bool hasHeader = s.find("Matrix[2x1]") != std::string::npos;
        bool hasValues = s.find("3") != std::string::npos && s.find("7") != std::string::npos;
        runTest(logger, "T16_toString_format", hasHeader && hasValues);
    }

    // Test 17: Self-assignment safety
    {
        DynamicMatrix m;
        m += 5;
        m = m;  // self-assign
        bool ok = (m.getRows() == 1) && (m.get(0, 0) == 5);
        runTest(logger, "T17_self_assignment_safe", ok);
    }

    // Test 18: Comparison chain consistency
    {
        DynamicMatrix a, b;
        a += 1;
        b += 1; b += 2;
        bool ok = (a < b) && (a <= b) && (b > a) && (b >= a) && !(a == b);
        runTest(logger, "T18_comparison_chain", ok);
    }

    logger.log("\n=== Test Suite Completed ===");
    logger.log("Total tests: 18");
    logger.log("Check log.txt for full output.");

    return 0;
}
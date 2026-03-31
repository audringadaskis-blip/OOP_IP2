#include "DynamicMatrix.h"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>  // dėl std::setw

using namespace AudrisMatrix;

// HELPER: Logeris į stdout ir failą
class TestLogger {
private:
    std::ofstream logFile;
    int passCount = 0;
    int failCount = 0;
    
public:
    TestLogger(const std::string& filename) {
        logFile.open(filename);
    }
    
    ~TestLogger() {
        if (logFile.is_open()) logFile.close();
    }
    
    // Paprastas pranešimas
    void log(const std::string& msg) {
        std::cout << msg << std::endl;
        if (logFile.is_open()) logFile << msg << std::endl;
    }
    
    // Testo rezultatas su detalia informacija
    void logTest(const std::string& description, bool passed, 
                 const std::string& expected = "", const std::string& actual = "") {
        std::string status = passed ? "✅ PASSED" : "❌ FAILED";
        
        if (passed) {
            passCount++;
            log("  [OK] " + description);
        } else {
            failCount++;
            std::string msg = "  [FAIL] " + description;
            if (!expected.empty()) {
                msg += "\n         Tikėtasi: " + expected;
            }
            if (!actual.empty()) {
                msg += "\n         Gauta:    " + actual;
            }
            log(msg);
        }
    }
    
    // Skyriaus antraštė
    void logSection(const std::string& title) {
        log("\nFailas: " + title);
        log(std::string(50, '-'));
    }
    
    // Galutinė statistika
    void logSummary() {
        int total = passCount + failCount;
        double percentage = total > 0 ? (100.0 * passCount / total) : 100.0;
        
        log("\n" + std::string(50, '='));
        log("REZULTATŲ SUVESTINĖ");
        log(std::string(50, '='));
        log("  Viso testų:    " + std::to_string(total));
        log("  Sėkmingi:      " + std::to_string(passCount) + " T");
        log("  Nepavyko:      " + std::to_string(failCount) + " F");
        log("  Sėkmė:         " + std::to_string((int)percentage) + "%");
        log(std::string(50, '='));
    }
    
    bool is_open() const { return logFile.is_open(); }
};

// MAIN: Testų vykdymas
int main() {
    TestLogger logger("log.txt");
    if (!logger.is_open()) {
        std::cerr << "Klaida: nepavyko atidaryti log.txt failo" << std::endl;
        return 1;
    }

    logger.log("╔════════════════════════════════════════╗");
    logger.log("║   DynamicMatrix – Testų Rezultatai     ║");
    logger.log("╚════════════════════════════════════════╝");
    logger.log("Data: " + std::string(__DATE__) + " " + std::string(__TIME__));

    // 1. MATRICOS KŪRIMAS
    logger.logSection("1. Matricos kūrimas ir inicializavimas");
    
    {
        DynamicMatrix m;
        logger.logTest(
            "Tuščia matrica: eilučių=0, stulpelių=0",
            m.getRows() == 0 && m.getCols() == 0,
            "rows=0, cols=0",
            "rows=" + std::to_string(m.getRows()) + ", cols=" + std::to_string(m.getCols())
        );
    }

    // 2. ELEMENTŲ ĮTERPIMAS (operator+=)
    logger.logSection("2. Elementų įterpimas (operator+=)");
    
    {
        DynamicMatrix m;
        m += 42;
        logger.logTest(
            "Įterpti vieną reikšmę (42) į tuščią matricą",
            m.getRows() == 1 && m.getCols() == 1 && m.get(0, 0) == 42,
            "rows=1, cols=1, value=42",
            "rows=" + std::to_string(m.getRows()) + ", value=" + std::to_string(m.get(0,0))
        );
    }
    
    {
        DynamicMatrix m;
        m += 1; m += 2; m += 3;
        logger.logTest(
            "Įterpti tris reikšmes iš eilės (1, 2, 3)",
            m.getRows() == 3 && m.get(2, 0) == 3,
            "3 eilutės, paskutinė reikšmė=3",
            "eilučių=" + std::to_string(m.getRows()) + ", paskutinė=" + std::to_string(m.get(2,0))
        );
    }

    // 3. ELEMENTŲ REDAGAVIMAS (operator*=)
    logger.logSection("3. Elementų redagavimas (operator*=)");
    
    {
        DynamicMatrix m;
        m += 5; m += 10;
        m *= 3;
        bool ok = (m.get(0, 0) == 15) && (m.get(1, 0) == 30);
        logger.logTest(
            "Padauginti visus elementus iš 3 (5→15, 10→30)",
            ok,
            "values: 15, 30",
            "values: " + std::to_string(m.get(0,0)) + ", " + std::to_string(m.get(1,0))
        );
    }

    // 4. REIKŠMIŲ PAIEŠKA (operator[])
    logger.logSection("4. Reikšmių paieška (operator[])");
    
    {
        DynamicMatrix m;
        m += 7; m += 14; m += 21;
        try {
            auto pos = m[14];
            logger.logTest(
                "Rasti reikšmę 14 – grąžinti poziciją (1, 0)",
                pos.first == 1 && pos.second == 0,
                "position: (1, 0)",
                "position: (" + std::to_string(pos.first) + ", " + std::to_string(pos.second) + ")"
            );
        } catch (...) {
            logger.logTest("Rasti reikšmę 14 – grąžinti poziciją (1, 0)", false, "found", "exception thrown");
        }
    }
    
    {
        DynamicMatrix m;
        m += 1; m += 2;
        bool caught = false;
        try {
            m[999];
        } catch (const MatrixException&) {
            caught = true;
        }
        logger.logTest(
            "Ieškoti nerastos reikšmės (999) – turi išmesti klaidą",
            caught,
            "MatrixException išmesta",
            caught ? "išmesta" : "neišmesta"
        );
    }

    // 5. EILUČIŲ TRYNIMAS (operator-=)
    logger.logSection("5. Eilučių trynimas (operator-=)");
    
    {
        DynamicMatrix m;
        m += 1; m += 2; m += 3;
        m -= 1;  // trinti vidurinę
        bool ok = (m.getRows() == 2) && (m.get(0, 0) == 1) && (m.get(1, 0) == 3);
        logger.logTest(
            "Ištrinti vidurinę eilutę (liko: 1, 3)",
            ok,
            "rows=2, values: 1, 3",
            "rows=" + std::to_string(m.getRows()) + ", values: " + std::to_string(m.get(0,0)) + ", " + std::to_string(m.get(1,0))
        );
    }
    
    {
        DynamicMatrix m;
        m += 1;
        bool caught = false;
        try {
            m -= 10;  // neegzistuojantis indeksas
        } catch (const MatrixException&) {
            caught = true;
        }
        logger.logTest(
            "Trinti neegzistuojantį indeksą (10) – turi išmesti klaidą",
            caught,
            "MatrixException išmesta",
            caught ? "išmesta" : "neišmesta"
        );
    }

    // 6. MATRICOS IŠVALYMAS (operator!)
    logger.logSection("6. Matricos išvalymas (operator!)");
    
    {
        DynamicMatrix m;
        m += 1; m += 2;
        !m;
        logger.logTest(
            "Išvalyti matricą – grįžti į tuščią būseną",
            m.getRows() == 0 && m.getCols() == 0,
            "rows=0, cols=0",
            "rows=" + std::to_string(m.getRows()) + ", cols=" + std::to_string(m.getCols())
        );
    }

    // 7. LYGINIMO OPERATORIAI
    logger.logSection("7. Lyginimo operatoriai (==, !=, <, <=, >, >=)");
    
    {
        DynamicMatrix m1, m2;
        m1 += 5; m1 += 10;
        m2 += 5; m2 += 10;
        logger.logTest(
            "Lygybė: dvi identiškos matricos (==)",
            m1 == m2,
            "true",
            m1 == m2 ? "true" : "false"
        );
    }
    
    {
        DynamicMatrix m1, m2;
        m1 += 5;
        m2 += 7;
        logger.logTest(
            "Nelygybė: skirtingos matricos (!=)",
            m1 != m2,
            "true",
            m1 != m2 ? "true" : "false"
        );
    }
    
    {
        DynamicMatrix small, large;
        small += 1;
        large += 1; large += 2; large += 3;
        logger.logTest(
            "Mažiau (<): lyginimas pagal elementų skaičių (1 < 3)",
            small < large,
            "true",
            small < large ? "true" : "false"
        );
    }
    
    {
        DynamicMatrix a, b;
        a += 1;
        b += 1; b += 2;
        bool chain = (a < b) && (a <= b) && (b > a) && (b >= a) && !(a == b);
        logger.logTest(
            "Operatorių grandinė: <, <=, >, >=, == veikia nuosekliai",
            chain,
            "visi palyginimai teisingi",
            chain ? "OK" : "klaida grandinėje"
        );
    }

    // 8. KOPIJAVIMAS IR PRISKIRIMAS
    logger.logSection("8. Kopijavimas ir priskyrimas (deep copy)");
    
    {
        DynamicMatrix m1;
        m1 += 100;
        DynamicMatrix m2(m1);  // copy constructor
        m2.set(0, 0, 999);     // keičiame kopiją
        bool independent = (m1.get(0, 0) == 100) && (m2.get(0, 0) == 999);
        logger.logTest(
            "Kopijos nepriklausomumas: keičiant kopiją, originalas nesikeičia",
            independent,
            "m1=100, m2=999",
            "m1=" + std::to_string(m1.get(0,0)) + ", m2=" + std::to_string(m2.get(0,0))
        );
    }
    
    {
        DynamicMatrix m1, m2;
        m1 += 42;
        m2 = m1;      // assignment operator
        m2 += 99;     // keičiame priskirtą objektą
        bool independent = (m1.getRows() == 1) && (m2.getRows() == 2);
        logger.logTest(
            "Priskyrimo nepriklausomumas: keičiant vieną, kitas nesikeičia",
            independent,
            "m1.rows=1, m2.rows=2",
            "m1.rows=" + std::to_string(m1.getRows()) + ", m2.rows=" + std::to_string(m2.getRows())
        );
    }
    
    {
        DynamicMatrix m;
        m += 5;
        m = m;  // savęs priskyrimas
        bool safe = (m.getRows() == 1) && (m.get(0, 0) == 5);
        logger.logTest(
            "Savęs priskyrimas (m = m) – neturi sugadinti duomenų",
            safe,
            "rows=1, value=5",
            "rows=" + std::to_string(m.getRows()) + ", value=" + std::to_string(m.get(0,0))
        );
    }

    // 9. RIBŲ TIKRINIMAS IR FORMATAS
    logger.logSection("9. Ribų tikrinimas ir išvedimo formatas");
    
    {
        DynamicMatrix m;
        m += 1;
        bool caught = false;
        try {
            m.get(10, 0);  // indeksas už ribų
        } catch (const MatrixException&) {
            caught = true;
        }
        logger.logTest(
            "get() su neegzistuojančiu indeksu – turi išmesti klaidą",
            caught,
            "MatrixException išmesta",
            caught ? "išmesta" : "neišmesta"
        );
    }
    
    {
        DynamicMatrix m;
        m += 3; m += 7;
        std::string s = m.toString();
        bool hasHeader = s.find("Matrix[2x1]") != std::string::npos;
        bool hasValues = s.find("3") != std::string::npos && s.find("7") != std::string::npos;
        logger.logTest(
            "toString() formatas: turi būti antraštė ir reikšmės",
            hasHeader && hasValues,
            "Matrix[2x1] su 3 ir 7",
            hasHeader && hasValues ? "OK" : "formatas neteisingas"
        );
    }

    // GALUTINĖ SUVESTINĖ
    logger.logSummary();
    logger.log("\n Testų vykdymas baigtas. Rezultatai įrašyti ir sukurtas log.txt");

    return 0;
}
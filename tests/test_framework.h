#pragma once

#include <cmath>
#include <iostream>
#include <string>

namespace testing {

inline int g_passed = 0;
inline int g_failed = 0;

inline void report(bool ok, const std::string& description) {
    if (ok) {
        g_passed++;
        std::cout << "  [PASS] " << description << "\n";
    } else {
        g_failed++;
        std::cout << "  [FAIL] " << description << "\n";
    }
}

inline void suite(const std::string& name) {
    std::cout << "\n== " << name << " ==\n";
}

inline void summary() {
    std::cout << "\n----------------------------------------\n";
    std::cout << (g_passed + g_failed) << " tests, "
              << g_passed << " passed, " << g_failed << " failed\n";
}

}

#define EXPECT_TRUE(cond, desc) ::testing::report((cond), desc)
#define EXPECT_EQ(a, b, desc) ::testing::report((a) == (b), desc)
#define EXPECT_NEAR(a, b, eps, desc) \
    ::testing::report(std::fabs((double)(a) - (double)(b)) < (eps), desc)

#define EXPECT_THROWS(expr, exc_type, desc)                 \
    do {                                                    \
        bool threw = false;                                 \
        try {                                                \
            expr;                                            \
        } catch (const exc_type&) {                          \
            threw = true;                                    \
        } catch (...) {                                      \
        }                                                    \
        ::testing::report(threw, desc);                      \
    } while (0)

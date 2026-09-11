#include "test_framework.h"

void run_lexer_tests();
void run_parser_tests();
void run_history_tests();
void run_format_tests();

int main() {
    std::cout << "Running calculator core-logic tests...\n";

    run_lexer_tests();
    run_parser_tests();
    run_history_tests();
    run_format_tests();

    testing::summary();
    return testing::g_failed == 0 ? 0 : 1;
}

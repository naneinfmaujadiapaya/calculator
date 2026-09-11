# Tests only touch the pure-logic core (lexer/parser/history), which has
# zero raylib dependency, so this target builds even on machines that don't
# have raylib installed yet.

add_executable(calculator_tests
    main.cpp
    test_lexer.cpp
    test_parser.cpp
    test_history.cpp
    test_format.cpp
    ${CMAKE_SOURCE_DIR}/src/core/lexer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/parser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/history.cpp
    ${CMAKE_SOURCE_DIR}/src/core/format.cpp
)

target_include_directories(calculator_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include/core
)

enable_testing()
add_test(NAME calculator_core_tests COMMAND calculator_tests)

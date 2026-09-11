cmake_minimum_required(VERSION 3.15)
project(calculator_cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# BUILD_APP: the real raylib calculator. BUILD_TESTS: the unit test suite,
# which only needs the pure-logic core (lexer/parser/history/format) and
# therefore has no raylib dependency -- handy for CI or for verifying the
# logic before raylib is even installed.
option(BUILD_APP "Build the raylib calculator app" ON)
option(BUILD_TESTS "Build the unit test suite (no raylib required)" OFF)

# USE_SYSTEM_RAYLIB: if you already have raylib installed system-wide
# (e.g. via your distro's package manager) and want CMake to use that
# instead of downloading + compiling raylib from source, turn this ON.
# It's OFF by default because FetchContent is what makes
# `cmake -B build && cmake --build build` "just work" on a fresh machine
# without asking the person to go install raylib-dev first.
option(USE_SYSTEM_RAYLIB "Link against a system-installed raylib instead of fetching one" OFF)

set(CORE_SOURCES
    src/core/lexer.cpp
    src/core/parser.cpp
    src/core/history.cpp
    src/core/format.cpp
)

if(BUILD_APP)
    if(USE_SYSTEM_RAYLIB)
        find_package(raylib REQUIRED)
    else()
        include(FetchContent)
        set(FETCHCONTENT_QUIET FALSE)
        # Desktop-only build settings for raylib itself: we don't need its
        # example/game modules, just the library.
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(
            raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG        5.5
            GIT_SHALLOW    TRUE
        )
        FetchContent_MakeAvailable(raylib)
    endif()

    add_executable(calculator
        src/main.cpp
        ${CORE_SOURCES}
        src/ui/assets.cpp
        src/ui/audio.cpp
        src/ui/button.cpp
        src/ui/ui.cpp
    )

    target_include_directories(calculator PRIVATE
        include/core
        include/ui
    )

    target_link_libraries(calculator raylib)

    if (APPLE)
        target_link_libraries(calculator "-framework IOKit" "-framework Cocoa" "-framework OpenGL")
    endif()

    include(GNUInstallDirs)

    # AssetPath() (src/ui/assets.cpp) falls back to this compiled-in path when
    # it can't find assets/ next to the executable -- i.e. for an installed
    # build. Matches the `install(DIRECTORY assets ...)` line below.
    target_compile_definitions(calculator PRIVATE
        CALC_ASSETS_INSTALL_DIR="${CMAKE_INSTALL_FULL_DATADIR}/calculator/assets/")

    # Copy bundled assets (fonts, ...) next to the freshly built binary so
    # `./build/calculator` works right away, with no install step needed.
    add_custom_command(TARGET calculator POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                $<TARGET_FILE_DIR:calculator>/assets
        COMMENT "Copying assets/ next to the calculator binary")

    # --- Linux packaging: `cmake --install` drops the binary, a .desktop
    # launcher entry, and an icon into standard XDG locations so the app
    # shows up in application menus like any normally-installed program
    # (default prefix /usr/local; pass -DCMAKE_INSTALL_PREFIX=/usr for a
    # system-wide install, or package it into an AppImage -- see README.md).
    if(UNIX AND NOT APPLE)
        install(TARGETS calculator RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
        install(FILES packaging/linux/calculator.desktop
                DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
        install(FILES packaging/linux/calculator.svg
                DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
    endif()

    # assets/ (fonts) travel with the binary on every platform, not just Linux.
    install(DIRECTORY assets/ DESTINATION ${CMAKE_INSTALL_DATADIR}/calculator/assets)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

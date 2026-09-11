cmake_minimum_required(VERSION 3.15)
project(calculator_android LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# The app has no separate Android-specific source tree: it reuses the exact
# same core/ and ui/ sources as the Linux/desktop build. APP_ROOT points
# back at the repository root (android/app/CMakeLists.txt -> android/app ->
# android -> repo root, i.e. two levels up).
set(APP_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raylib)

# Built as a shared library (not an executable) because that's what
# android.app.NativeActivity loads: AndroidManifest.xml's
# android:value="calculator" meta-data must match this target's name.
add_library(calculator SHARED
    ${APP_ROOT}/src/main.cpp
    ${APP_ROOT}/src/core/lexer.cpp
    ${APP_ROOT}/src/core/parser.cpp
    ${APP_ROOT}/src/core/history.cpp
    ${APP_ROOT}/src/core/format.cpp
    ${APP_ROOT}/src/ui/assets.cpp
    ${APP_ROOT}/src/ui/audio.cpp
    ${APP_ROOT}/src/ui/button.cpp
    ${APP_ROOT}/src/ui/ui.cpp
)

target_include_directories(calculator PRIVATE
    ${APP_ROOT}/include/core
    ${APP_ROOT}/include/ui
)

target_link_libraries(calculator raylib log android)

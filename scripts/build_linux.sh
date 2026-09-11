#!/usr/bin/env bash
# Convenience build script for Linux.
#
# First run downloads and compiles raylib from source via CMake
# FetchContent, so it takes a few minutes; subsequent runs are fast
# (only rebuilds what changed).
#
# Usage:
#   ./scripts/build_linux.sh            # build the app
#   ./scripts/build_linux.sh --run      # build and immediately launch it
#   ./scripts/build_linux.sh --install  # build, then `cmake --install`
#                                        # (add the app to your app menu)
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/.."

BUILD_DIR="build"

echo "==> Configuring (Release)"
cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release -DBUILD_APP=ON

echo "==> Building"
cmake --build "$BUILD_DIR" -j"$(nproc)"

if [[ "${1:-}" == "--install" ]]; then
    echo "==> Installing (sudo, system-wide to /usr/local)"
    sudo cmake --install "$BUILD_DIR"
    echo "Done -- 'Calculator' should now show up in your app menu."
    exit 0
fi

echo "Built: $BUILD_DIR/calculator"

if [[ "${1:-}" == "--run" ]]; then
    exec "$BUILD_DIR/calculator"
fi

#pragma once
#include <string>

// Resolves a path to a bundled asset (e.g. "fonts/Inter-Regular.ttf") to an
// actual file on disk, trying a few likely locations in order:
//
//   1. <dir of the running executable>/assets/<relativePath>
//      (works right after `cmake --build`, no install needed)
//   2. <CMAKE_INSTALL_PREFIX>/share/calculator/assets/<relativePath>
//      (works for a `cmake --install`'d build, baked in at compile time)
//
// Returns an empty string if the asset can't be found anywhere, so callers
// can fall back to raylib's built-in default font instead of crashing.
std::string AssetPath(const std::string& relativePath);

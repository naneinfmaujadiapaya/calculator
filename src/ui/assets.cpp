#include "assets.h"
#include "raylib.h"

#ifndef CALC_ASSETS_INSTALL_DIR
#define CALC_ASSETS_INSTALL_DIR ""
#endif

#if defined(__ANDROID__)

std::string AssetPath(const std::string& relativePath) {
    // On Android raylib transparently reads through AAssetManager against
    // whatever Gradle packaged into the APK's assets/ folder (see
    // android/app/build.gradle's sourceSets.main.assets.srcDirs, which
    // points at the same top-level assets/ this repo uses on desktop) --
    // there's no real filesystem path to resolve, just hand back the
    // relative name as-is.
    return relativePath;
}

#else

std::string AssetPath(const std::string& relativePath) {
    // 1) Next to the executable (covers a plain `cmake --build` run, where
    //    CMake copies assets/ alongside the binary post-build).
    std::string nextToExe = std::string(GetApplicationDirectory()) + "assets/" + relativePath;
    if (FileExists(nextToExe.c_str())) return nextToExe;

    // 2) The install-time location baked in by CMake (e.g.
    //    /usr/local/share/calculator/assets/... after `cmake --install`).
    std::string installed = std::string(CALC_ASSETS_INSTALL_DIR) + relativePath;
    if (!installed.empty() && FileExists(installed.c_str())) return installed;

    return "";
}

#endif

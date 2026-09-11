Calculator C++

A scientific desktop (and Android) calculator written in C++17 using raylib, with its own hand-written lexer/parser instead of `eval()`.

Built from scratch by naneinf as a personal project to learn and experiment with C++, parsing, UI development, testing, and cross-platform builds.

«Project status: In development»

<img width="632" height="651" alt="calculator_basic_mode" src="https://github.com/user-attachments/assets/96fea29d-d68e-48c8-985c-d8f34e95bbf8" />

<img width="1065" height="706" alt="calculator_scientific_mode" src="https://github.com/user-attachments/assets/f573f23d-f660-4957-baa1-40165ea7599b" />


# Features

-  Expression-based calculator
-  Custom lexer for tokenizing mathematical expressions
-  Custom parser for evaluating expressions, with full operator precedence and parentheses
-  Scientific functions: sqrt, cbrt, sin/cos/tan, asin/acos/atan, sinh/cosh/tanh, ln, log, abs, factorial (!)
-  Combinatorics/number theory: nCr, nPr, gcd, lcm
-  Constants (pi, e) and "Ans" (previous result)
-  Degree/Radian toggle for trig functions
-  Memory keys: MC / MR / MS / M+ / M-
-  Calculation history, saved to disk so it survives restarting the app
-  Copy result to clipboard
-  Number formatting
-  Frameless, macOS-calculator-style window you drag by clicking anywhere on the title bar, with real traffic-light controls (close / minimize / Basic <-> Scientific)
-  UI text set in the Inter typeface (bundled, not system-dependent) instead of raylib's default bitmap font
-  Graphical interface powered by raylib, with a Basic/Scientific keypad toggle
-  Audio/UI feedback
-  Unit tests for the calculator's core logic (120+ cases)
-  Linux installation / desktop integration
-  Android build setup, with CI that auto-builds an installable .apk (see "Android" below)
-  CMake-based build system

# Requirements

- CMake 3.15+
- A C++ compiler with C++17 support
- Git
- raylib 5.5 (automatically downloaded by CMake by default)

For running the test suite, raylib is not required.

# Building

Clone the repository:

git clone https://github.com/naneinfmaujadiapaya/calculator.git
cd calculator

Create a build directory:

cmake -B build

Build the application:

cmake --build build

Then run:

./build/calculator

(`cmake --build` also copies `assets/` -- the bundled fonts -- next to the binary automatically, so this works right away with no extra step.)

Using system raylib

If raylib is already installed on your system:

cmake -B build -DUSE_SYSTEM_RAYLIB=ON
cmake --build build

# Running Tests

The core calculator logic is separated from the raylib UI, so the tests can be built without raylib.

cmake -B build-tests -DBUILD_APP=OFF -DBUILD_TESTS=ON
cmake --build build-tests
ctest --test-dir build-tests --output-on-failure

The test suite currently covers:

- Lexer
- Parser
- History
- Number formatting

# How It Works

The calculator doesn't simply throw the input directly at a mathematical function.

The expression goes through several stages:

User Input
    ↓
Lexer
    ↓
Tokens
    ↓
Parser
    ↓
Evaluation
    ↓
Formatter
    ↓
Result

This separation keeps the calculator's mathematical logic independent from the graphical interface.

# Interface

The window has no OS title bar -- it's drawn by the app itself (`FLAG_WINDOW_UNDECORATED`), macOS-calculator-style:

- **Drag to move**: click anywhere on the dark strip at the top and drag. There's no OS-level resize border, since the window already resizes itself when you flip Basic/Scientific.
- **Traffic lights**, top-left:
  - Red -- close the app
  - Yellow -- minimize
  - Green -- toggle Basic/Scientific (same as the "Scientific"/"Basic" button below the display)
- **Escape** now only clears the input (`C`); it no longer closes the window, since that's what the red dot is for.

Text is set in [Inter](https://github.com/rsms/inter) (SIL Open Font License, bundled under `assets/fonts/`, see `OFL.txt` there) at two weights instead of raylib's built-in bitmap font -- crisper at every size, and buttons auto-shrink long labels (`sqrt`, `nCr`, ...) so nothing clips in Scientific mode.

# Linux

The project includes a Linux desktop entry and application icon.

You can install the application using CMake:

cmake --install build

Or use a custom installation prefix:

cmake --install build --prefix ~/.local

# Android

An Android build configuration is included in:

android/

The Android target reuses the same calculator core instead of maintaining a completely separate implementation, including the same `assets/fonts/` used by the desktop build (wired up via `sourceSets.main.assets` in `android/app/build.gradle`).

Getting an APK without installing Android Studio: every push to `main` runs `.github/workflows/android-build.yml` on GitHub's own runners (which already have the Android SDK/NDK), and uploads the built `.apk` as a workflow artifact under the repo's Actions tab. Pushing a tag like `v1.0` also attaches the APK directly to a GitHub Release, so `Releases -> latest -> calculator.apk` is a stable "just download" link.

To build the APK yourself instead (e.g. to sign it for the Play Store), open the `android/` folder in Android Studio, or run:

cd android
gradle assembleDebug

The debug build is signed with Android's built-in debug key so it installs straight away; a release build needs its own signing key before Android will install it.

# Tech Stack

Technology| Purpose
C++17| Main programming language
raylib 5.5| Graphics & UI
Inter (OFL-1.1)| UI typeface, bundled under assets/fonts/
CMake| Build system
Git| Version control
Android NDK| Android build
CTest| Testing

# Why I Made This

This project started as a way to learn C++ by actually building something instead of only following tutorials.

The goal isn't to compete with professional calculator software.

It's a personal project focused on learning:

- C++ architecture
- Lexical analysis
- Parsing
- Separation of concerns
- GUI programming
- Unit testing
- CMake
- Cross-platform development

And yes, I probably overengineered a calculator.

# Roadmap

Possible future improvements:

- [x] More mathematical operators
- [x] Parentheses and more complex expressions
- [x] Better error messages
- [x] Keyboard input
- [x] Scientific functions, memory keys, degree/radian mode
- [x] CI that builds a downloadable Android APK automatically
- [x] Custom frameless window + bundled typeface (Inter) instead of the default bitmap font
- [ ] Unit converter / currency converter
- [ ] Persistent history (saved across app restarts)
- [ ] Theming (light mode / accent color picker), haptics on Android
- [ ] More comprehensive tests
- [ ] Signed release builds for the Play Store
- [ ] Packaging for more Linux distributions

# Author

naneinf

This project was designed, written, and developed by me.

«Built with C++, questionable decisions, and an unreasonable amount of time spent making a calculator.»

---

# If you find the project interesting, feel free to explore the source code.

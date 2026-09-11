#include "raylib.h"
#include <string>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "lexer.h"
#include "parser.h"
#include "history.h"
#include "format.h"
#include "audio.h"
#include "ui.h"

namespace {

std::string input;
std::string resultText = "0";
bool isError = false;
History history;
AudioManager audio;

bool useDegrees = false;
double memoryValue = 0.0;
bool hasMemory = false;
double lastAnswer = 0.0;

std::string HistoryFilePath() {
    return std::string(GetApplicationDirectory()) + "calculator_history.txt";
}

void SaveHistory() {
    std::ofstream out(HistoryFilePath(), std::ios::trunc);
    if (!out.is_open()) return;
    // presisi 17 digit biar double-nya balik persis pas dibaca lagi
    out << std::setprecision(17);
    for (const auto& entry : history.entries()) {
        out << entry.expression << '\t' << entry.result << '\n';
    }
}

void LoadHistory() {
    std::ifstream in(HistoryFilePath());
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line)) {
        auto tab = line.find('\t');
        if (tab == std::string::npos) continue;
        std::string expr = line.substr(0, tab);
        try {
            double value = std::stod(line.substr(tab + 1));
            history.add(expr, value);
        } catch (const std::exception&) {
            // baris rusak dilewati aja, gak boleh bikin app crash
        }
    }
}

std::string AutoCloseParens(const std::string& expr) {
    int depth = 0;
    for (char c : expr) {
        if (c == '(') depth++;
        else if (c == ')' && depth > 0) depth--;
    }
    return expr + std::string(depth, ')');
}

bool TryEvaluateCurrent(double& out) {
    std::string expr = input.empty() ? resultText : input;
    try {
        Lexer lexer(AutoCloseParens(expr));
        Parser parser(lexer.tokenize(), lastAnswer,
                       useDegrees ? AngleMode::Degrees : AngleMode::Radians);
        out = parser.parse();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void Evaluate() {
    if (input.empty()) return;
    try {
        Lexer lexer(AutoCloseParens(input));
        Parser parser(lexer.tokenize(), lastAnswer,
                       useDegrees ? AngleMode::Degrees : AngleMode::Radians);
        double value = parser.parse();
        resultText = FormatNumber(value);
        isError = false;
        history.add(input, value);
        SaveHistory();
        lastAnswer = value;
        audio.PlayEquals();
        input = resultText;
    } catch (const std::exception& e) {
        resultText = e.what();
        isError = true;
        audio.PlayError();
    }
}

void HandleKey(const std::string& key) {
    if (key == "C") {
        input.clear();
        resultText = "0";
        isError = false;
        audio.PlayClear();
        return;
    }
    if (key == "=") {
        Evaluate();
        return;
    }
    if (key == "DEL") {
        if (isError) { input.clear(); isError = false; }
        else if (!input.empty()) input.pop_back();
        audio.PlayClear();
        return;
    }
    if (key == "ANS") {
        if (isError) { input.clear(); isError = false; }
        input += FormatNumber(lastAnswer);
        audio.PlayOperator();
        return;
    }
    if (key == "DEGRAD") {
        useDegrees = !useDegrees;
        audio.PlayOperator();
        return;
    }
    if (key == "COPY") {
        SetClipboardText(resultText.c_str());
        audio.PlayOperator();
        return;
    }
    if (key == "MC") {
        memoryValue = 0.0;
        hasMemory = false;
        audio.PlayClear();
        return;
    }
    if (key == "MR") {
        if (isError) { input.clear(); isError = false; }
        input += FormatNumber(memoryValue);
        audio.PlayOperator();
        return;
    }
    if (key == "MS" || key == "M+" || key == "M-") {
        double value;
        if (TryEvaluateCurrent(value)) {
            if (key == "MS") memoryValue = value;
            else if (key == "M+") memoryValue += value;
            else memoryValue -= value;
            hasMemory = true;
            audio.PlayOperator();
        } else {
            audio.PlayError();
        }
        return;
    }

    if (isError) {
        input.clear();
        isError = false;
    }

    input += key;

    if (key.size() == 1 && std::isdigit(static_cast<unsigned char>(key[0]))) {
        audio.PlayDigit(key[0] - '0');
    } else {
        audio.PlayOperator();
    }
}

void HandleKeyboardInput() {
    int c = GetCharPressed();
    while (c > 0) {
        if ((c >= '0' && c <= '9') || c == '.' || c == '+' || c == '-' ||
            c == '*' || c == '/' || c == '%' || c == '^' || c == '(' || c == ')' ||
            c == '!' || c == ',') {
            HandleKey(std::string(1, static_cast<char>(c)));
        }
        c = GetCharPressed();
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) Evaluate();
    if (IsKeyPressed(KEY_BACKSPACE)) HandleKey("DEL");
    if (IsKeyPressed(KEY_ESCAPE)) HandleKey("C");
}

}

int main() {
    // Frameless, macOS-calculator-style window: no OS title bar/border, we
    // draw and handle our own (see CalculatorUI::DrawTitleBar/UpdateTitleBar).
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_MSAA_4X_HINT);
    InitWindow(CalculatorUI::ScreenWidthBasic,
               CalculatorUI::ContentHeightBasic + static_cast<int>(CalculatorUI::TitleBarHeight),
               "Calculator");
    // Escape is repurposed as the "C" (clear) key, so stop it from also
    // triggering raylib's default WindowShouldClose()-on-ESC behavior.
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    audio.Init();
    LoadHistory();

    CalculatorUI ui;
    ui.Init();
    ui.BuildButtons(HandleKey);

    while (!WindowShouldClose() && !ui.CloseRequested()) {
        if (GetScreenWidth() != ui.Width() || GetScreenHeight() != ui.Height()) {
            SetWindowSize(ui.Width(), ui.Height());
        }

        ui.Update();
        HandleKeyboardInput();

        BeginDrawing();
        ui.Draw(input, resultText, isError, history, useDegrees, hasMemory, lastAnswer);
        EndDrawing();
    }

    audio.Shutdown();
    CloseWindow();
    return 0;
}

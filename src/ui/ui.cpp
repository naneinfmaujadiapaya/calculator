#include "ui.h"
#include "format.h"
#include "assets.h"
#include <cmath>

namespace {
constexpr Color kBg        = {22, 23, 31, 255};
constexpr Color kTitleBg   = {16, 17, 23, 255};
constexpr Color kDisplayBg = {30, 32, 43, 255};
constexpr Color kDigitBtn  = {54, 58, 74, 255};
constexpr Color kOpBtn     = {247, 148, 32, 255};
constexpr Color kEqualsBtn = {88, 199, 138, 255};
constexpr Color kClearBtn  = {224, 82, 82, 255};
constexpr Color kFuncBtn   = {66, 96, 128, 255};
constexpr Color kMemBtn    = {112, 92, 156, 255};
constexpr Color kMuted     = {148, 153, 168, 255};
constexpr Color kAccent    = {88, 199, 138, 255};
constexpr Color kHistoryBg = {17, 18, 24, 255};
constexpr Color kDivider   = {40, 42, 54, 255};

// Authentic macOS traffic-light colors.
constexpr Color kDotClose    = {255, 95, 87, 255};
constexpr Color kDotMinimize = {254, 188, 46, 255};
constexpr Color kDotZoom     = {40, 200, 64, 255};
constexpr float kDotRadius = 6.0f;
constexpr float kDotSpacing = 20.0f;
constexpr float kDotStartX = 18.0f;

const char* kBasicLayout[5][4] = {
    {"C",  "(",  ")",  "%"},
    {"7",  "8",  "9",  "/"},
    {"4",  "5",  "6",  "*"},
    {"1",  "2",  "3",  "-"},
    {"0",  ".",  "=",  "+"},
};

// tombol fungsi ngirim nama plus kurung buka, sisanya di-auto-close pas '='
struct SciKey { const char* label; const char* send; };
const SciKey kSciLayout[6][4] = {
    {{"Rad", "DEGRAD"}, {"sin", "sin("},  {"cos", "cos("},  {"tan", "tan("}},
    {{"ln", "ln("},     {"log", "log("},  {"sqrt", "sqrt("},{"cbrt", "cbrt("}},
    {{"x^2", "^2"},     {"1/x", "^-1"},   {"n!", "!"},      {"pi", "pi"}},
    {{"e", "e"},        {"nCr", "ncr("},  {"nPr", "npr("},  {"gcd", "gcd("}},
    {{"lcm", "lcm("},   {"Ans", "ANS"},   {"DEL", "DEL"},   {"MC", "MC"}},
    {{"MR", "MR"},      {"MS", "MS"},     {"M+", "M+"},     {"M-", "M-"}},
};

Font LoadBundledFont(const char* relativePath, int baseSize) {
    std::string path = AssetPath(relativePath);
    if (!path.empty()) {
        // Bake a generous, fixed-size ASCII atlas once, up front, instead of
        // relying on raylib's default 10px font stretched with DrawTextEx --
        // that's what made the old UI look fuzzy/pixelated.
        Font loaded = LoadFontEx(path.c_str(), baseSize, nullptr, 0);
        if (loaded.texture.id != 0 && loaded.glyphCount > 0) {
            SetTextureFilter(loaded.texture, TEXTURE_FILTER_BILINEAR);
            return loaded;
        }
    }
    return GetFontDefault();
}
}

void CalculatorUI::Init() {
    font = LoadBundledFont("fonts/Inter-Regular.ttf", 48);
    fontDisplay = LoadBundledFont("fonts/Inter-SemiBold.ttf", 48);
}

void CalculatorUI::BuildBasicGrid(float gridLeft, float gridTop, float cellW, float cellH,
                                   float gap) {
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 4; col++) {
            std::string label = kBasicLayout[row][col];
            Button b;
            b.bounds = {
                gridLeft + col * (cellW + gap),
                gridTop + row * (cellH + gap),
                cellW, cellH
            };
            b.label = label;

            if (label == "=") {
                b.baseColor = kEqualsBtn;
                b.textColor = kBg;
            } else if (label == "C") {
                b.baseColor = kClearBtn;
            } else if (label == "+" || label == "-" || label == "*" ||
                       label == "/" || label == "%") {
                b.baseColor = kOpBtn;
            } else {
                b.baseColor = kDigitBtn;
            }

            auto onKey = onKeyCallback;
            b.onClick = [onKey, label]() { onKey(label); };
            buttons.push_back(b);
        }
    }
}

void CalculatorUI::BuildScientificGrid(float gridLeft, float gridTop, float cellW, float cellH,
                                        float gap) {
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 4; col++) {
            SciKey key = kSciLayout[row][col];
            Button b;
            b.bounds = {
                gridLeft + col * (cellW + gap),
                gridTop + row * (cellH + gap),
                cellW, cellH
            };
            b.label = key.label;

            std::string send = key.send;
            if (send == "MC" || send == "MR" || send == "MS" || send == "M+" || send == "M-" ||
                send == "DEGRAD") {
                b.baseColor = kMemBtn;
            } else if (send == "ANS" || send == "DEL") {
                b.baseColor = kClearBtn;
            } else {
                b.baseColor = kFuncBtn;
            }

            auto onKey = onKeyCallback;
            b.onClick = [onKey, send]() { onKey(send); };
            buttons.push_back(b);
        }
    }
}

void CalculatorUI::BuildButtons(const std::function<void(const std::string&)>& onKey) {
    onKeyCallback = onKey;
    buttons.clear();

    const float contentTop = TitleBarHeight;
    const float panelWidth = 200.0f;
    const float gridLeft = 20.0f;
    const float gridTop = contentTop + 150.0f;
    const float gap = 10.0f;
    const int totalCols = scientificMode ? 8 : 4;
    const float cellW = (Width() - panelWidth - gridLeft * 2 - (totalCols - 1) * gap) / totalCols;
    const float cellH = 70.0f;

    float basicLeft = gridLeft;
    if (scientificMode) {
        BuildScientificGrid(gridLeft, gridTop, cellW, cellH, gap);
        basicLeft = gridLeft + 4 * (cellW + gap);
    }
    BuildBasicGrid(basicLeft, gridTop, cellW, cellH, gap);

    sciToggle.bounds = {gridLeft, contentTop + 118, 112, 28};
    sciToggle.label = scientificMode ? "Basic" : "Scientific";
    sciToggle.baseColor = kAccent;
    sciToggle.textColor = kBg;
    sciToggle.onClick = [this]() {
        scientificMode = !scientificMode;
        BuildButtons(onKeyCallback);
    };

    copyButton.bounds = {gridLeft + 122, contentTop + 118, 90, 28};
    copyButton.label = "Copy";
    copyButton.baseColor = kFuncBtn;
    copyButton.textColor = WHITE;
    copyButton.onClick = [onKey]() { onKey("COPY"); };
}

Vector2 CalculatorUI::TrafficLightCenter(int index) const {
    return {kDotStartX + index * kDotSpacing, TitleBarHeight / 2.0f};
}

void CalculatorUI::UpdateTitleBar() {
    Vector2 mouse = GetMousePosition();

    Vector2 closeC = TrafficLightCenter(0);
    Vector2 minC   = TrafficLightCenter(1);
    Vector2 zoomC  = TrafficLightCenter(2);
    float hitR = kDotRadius + 4.0f;

    bool overClose = CheckCollisionPointCircle(mouse, closeC, hitR);
    bool overMin   = CheckCollisionPointCircle(mouse, minC, hitR);
    bool overZoom  = CheckCollisionPointCircle(mouse, zoomC, hitR);

    if (overClose || overMin || overZoom) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (overClose) {
            closeRequested = true;
            return;
        }
        if (overMin) {
            MinimizeWindow();
            return;
        }
        if (overZoom) {
            scientificMode = !scientificMode;
            BuildButtons(onKeyCallback);
            return;
        }
    }

    // Drag-to-move: click anywhere else on the title bar strip and drag.
    Rectangle barRect = {0, 0, static_cast<float>(Width()), TitleBarHeight};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, barRect) &&
        !overClose && !overMin && !overZoom) {
        dragging = true;
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        dragging = false;
    }
    if (dragging) {
        Vector2 delta = GetMouseDelta();
        if (delta.x != 0 || delta.y != 0) {
            Vector2 pos = GetWindowPosition();
            SetWindowPosition(static_cast<int>(pos.x + delta.x),
                               static_cast<int>(pos.y + delta.y));
        }
    }
}

void CalculatorUI::Update() {
    UpdateTitleBar();
    sciToggle.HandleClick();
    copyButton.HandleClick();
    for (auto& b : buttons) {
        b.HandleClick();
    }

    if (!dragging) {
        bool overAny = sciToggle.IsHovered() || copyButton.IsHovered();
        for (auto& b : buttons) overAny = overAny || b.IsHovered();
        SetMouseCursor(overAny ? MOUSE_CURSOR_POINTING_HAND : MOUSE_CURSOR_DEFAULT);
    }
}

void CalculatorUI::DrawTitleBar() const {
    DrawRectangle(0, 0, Width(), static_cast<int>(TitleBarHeight), kTitleBg);
    DrawLine(0, static_cast<int>(TitleBarHeight), Width(), static_cast<int>(TitleBarHeight),
             kDivider);

    Vector2 mouse = GetMousePosition();
    struct Dot { Vector2 center; Color color; const char* glyph; };
    Dot dots[3] = {
        {TrafficLightCenter(0), kDotClose,    "x"},
        {TrafficLightCenter(1), kDotMinimize, "-"},
        {TrafficLightCenter(2), kDotZoom,     scientificMode ? "v" : "^"},
    };
    for (const auto& d : dots) {
        bool hovered = CheckCollisionPointCircle(mouse, d.center, kDotRadius + 4.0f);
        DrawCircleV(d.center, kDotRadius, d.color);
        if (hovered) {
            Vector2 gs = MeasureTextEx(fontDisplay, d.glyph, 9, 0);
            DrawTextEx(fontDisplay, d.glyph,
                       {d.center.x - gs.x / 2.0f, d.center.y - gs.y / 2.0f}, 9, 0,
                       {40, 20, 15, 200});
        }
    }

    const char* title = "Calculator";
    Vector2 ts = MeasureTextEx(font, title, 14, 0.5f);
    DrawTextEx(font, title, {(Width() - ts.x) / 2.0f, (TitleBarHeight - ts.y) / 2.0f}, 14, 0.5f,
               kMuted);
}

void CalculatorUI::DrawDisplay(const std::string& input, const std::string& resultText,
                                bool isError, bool useDegrees, bool hasMemory,
                                double ansValue) const {
    const float contentTop = TitleBarHeight;
    Rectangle displayBox = {20, contentTop + 20, static_cast<float>(Width()) - 200 - 40, 90};
    DrawRectangleRounded(displayBox, 0.18f, 8, kDisplayBg);

    std::string status = useDegrees ? "DEG" : "RAD";
    if (hasMemory) status += "   M";
    DrawTextEx(font, status.c_str(), {displayBox.x + 14, displayBox.y + 8}, 13, 0.5f, kMuted);
    (void)ansValue;

    std::string shownInput = input.empty() ? "0" : input;
    Vector2 inputSize = MeasureTextEx(font, shownInput.c_str(), 22, 0.5f);
    DrawTextEx(font, shownInput.c_str(),
               {displayBox.x + displayBox.width - inputSize.x - 16, displayBox.y + 22},
               22, 0.5f, kMuted);

    Color resultColor = isError ? kClearBtn : WHITE;
    Vector2 resultSize = MeasureTextEx(fontDisplay, resultText.c_str(), 36, 0.5f);
    DrawTextEx(fontDisplay, resultText.c_str(),
               {displayBox.x + displayBox.width - resultSize.x - 16, displayBox.y + 50},
               36, 0.5f, resultColor);
}

void CalculatorUI::DrawHistoryPanel(const History& history) const {
    const float contentTop = TitleBarHeight;
    Rectangle panel = {static_cast<float>(Width()) - 200.0f, contentTop, 200,
                        static_cast<float>(Height()) - contentTop};
    DrawRectangleRec(panel, kHistoryBg);
    DrawLine(static_cast<int>(panel.x), static_cast<int>(contentTop),
             static_cast<int>(panel.x), Height(), kDivider);
    DrawTextEx(fontDisplay, "History", {panel.x + 16, contentTop + 16}, 19, 0.3f, kMuted);

    const auto& items = history.entries();
    float y = contentTop + 54;
    for (auto it = items.rbegin(); it != items.rend() && y < Height() - 40; ++it) {
        std::string exprLine = it->expression;
        std::string resultLine = "= " + FormatNumber(it->result);

        DrawTextEx(font, exprLine.c_str(), {panel.x + 16, y}, 14, 0.3f, kMuted);
        DrawTextEx(fontDisplay, resultLine.c_str(), {panel.x + 16, y + 18}, 16, 0.3f, WHITE);
        y += 48;
    }

    if (items.empty()) {
        DrawTextEx(font, "No history yet", {panel.x + 16, contentTop + 54}, 13, 0.3f, kDivider);
    }
}

void CalculatorUI::Draw(const std::string& input, const std::string& resultText, bool isError,
                         const History& history, bool useDegrees, bool hasMemory,
                         double ansValue) {
    ClearBackground(kBg);
    DrawDisplay(input, resultText, isError, useDegrees, hasMemory, ansValue);
    DrawHistoryPanel(history);
    sciToggle.Draw(font);
    copyButton.Draw(font);
    for (auto& b : buttons) {
        b.Draw(fontDisplay);
    }
    DrawTitleBar();
}

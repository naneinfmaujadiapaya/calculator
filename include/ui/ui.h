#pragma once
#include <vector>
#include <string>
#include <functional>
#include "raylib.h"
#include "button.h"
#include "history.h"

class CalculatorUI {
public:
    // Content height (display + keypad + history), NOT counting the custom
    // title bar -- TitleBarHeight gets added on top of these at Height().
    static constexpr int ContentHeightBasic = 620;
    static constexpr int ContentHeightScientific = 680;
    static constexpr int ScreenWidthBasic = 640;
    static constexpr int ScreenWidthScientific = 1080;
    static constexpr float TitleBarHeight = 38.0f;

    int Width() const { return scientificMode ? ScreenWidthScientific : ScreenWidthBasic; }
    int Height() const {
        return (scientificMode ? ContentHeightScientific : ContentHeightBasic) +
               static_cast<int>(TitleBarHeight);
    }
    bool IsScientific() const { return scientificMode; }

    // True once the user has clicked the (custom, undecorated-window) close
    // dot in the title bar -- main() should treat this like WindowShouldClose().
    bool CloseRequested() const { return closeRequested; }

    void Init();
    void BuildButtons(const std::function<void(const std::string&)>& onKey);
    void Update();
    void Draw(const std::string& input, const std::string& resultText, bool isError,
              const History& history, bool useDegrees, bool hasMemory, double ansValue);

private:
    std::vector<Button> buttons;
    Button sciToggle;
    Button copyButton;
    Font font{};        // Inter Regular -- body text, history, status line
    Font fontDisplay{};  // Inter SemiBold -- button labels + the big result
    bool scientificMode = false;
    bool closeRequested = false;
    bool dragging = false;
    std::function<void(const std::string&)> onKeyCallback;

    void BuildBasicGrid(float gridLeft, float gridTop, float cellW, float cellH, float gap);
    void BuildScientificGrid(float gridLeft, float gridTop, float cellW, float cellH, float gap);
    void DrawDisplay(const std::string& input, const std::string& resultText, bool isError,
                      bool useDegrees, bool hasMemory, double ansValue) const;
    void DrawHistoryPanel(const History& history) const;

    // macOS-style traffic lights + click-drag-anywhere-on-the-bar title bar,
    // needed because the window is created with FLAG_WINDOW_UNDECORATED.
    void UpdateTitleBar();
    void DrawTitleBar() const;
    Vector2 TrafficLightCenter(int index) const;
};

#pragma once
#include <string>
#include <functional>
#include "raylib.h"

struct Button {
    Rectangle bounds{};
    std::string label;
    Color baseColor = DARKGRAY;
    Color textColor = WHITE;
    std::function<void()> onClick;

    bool IsHovered() const;
    void Draw(const Font& font) const;
    bool HandleClick();
};

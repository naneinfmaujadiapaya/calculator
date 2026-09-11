#include "button.h"

bool Button::IsHovered() const {
    return CheckCollisionPointRec(GetMousePosition(), bounds);
}

void Button::Draw(const Font& font) const {
    bool hovered = IsHovered();
    bool pressed = hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    // Soft drop shadow first, so keys read as slightly raised "keycaps".
    Rectangle shadow = bounds;
    shadow.y += pressed ? 1.0f : 3.0f;
    DrawRectangleRounded(shadow, 0.32f, 8, Fade(BLACK, pressed ? 0.10f : 0.22f));

    Rectangle face = bounds;
    if (pressed) face.y += 1.5f;

    Color fill = baseColor;
    if (pressed) {
        fill = ColorBrightness(baseColor, -0.22f);
    } else if (hovered) {
        fill = ColorBrightness(baseColor, 0.12f);
    }

    DrawRectangleRounded(face, 0.32f, 8, fill);

    // Long labels ("sqrt", "cbrt", "DEL", ...) need a smaller size than a
    // single digit to avoid clipping the key.
    float fontSize = 25.0f;
    Vector2 textSize = MeasureTextEx(font, label.c_str(), fontSize, 0.5f);
    while (textSize.x > face.width * 0.78f && fontSize > 13.0f) {
        fontSize -= 1.5f;
        textSize = MeasureTextEx(font, label.c_str(), fontSize, 0.5f);
    }

    Vector2 textPos = {
        face.x + (face.width - textSize.x) / 2.0f,
        face.y + (face.height - textSize.y) / 2.0f
    };
    DrawTextEx(font, label.c_str(), textPos, fontSize, 0.5f, textColor);
}

bool Button::HandleClick() {
    if (IsHovered() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (onClick) onClick();
        return true;
    }
    return false;
}

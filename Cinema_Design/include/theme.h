#pragma once
#include "raylib.h"

enum class AppTheme {
    Dark,
    Light
};

struct ThemePalette {
    Color background;
    Color card;
    Color cardBg;
    Color cardHover;
    Color border;
    Color text;
    Color textDim;
    Color accent;
    Color accentDim;
    Color accentHover;
    Color pink;
    Color pinkHover;
    Color danger;
    Color dangerHover;
    Color success;
    Color seatAvailable;
    Color seatHover;
};

inline AppTheme currentTheme = AppTheme::Dark;

inline bool IsLightTheme() {
    return currentTheme == AppTheme::Light;
}

inline void ToggleTheme() {
    currentTheme = IsLightTheme() ? AppTheme::Dark : AppTheme::Light;
}

inline const ThemePalette& GetTheme() {
    static const ThemePalette dark = {
        { 15, 23, 42, 255 },
        { 30, 41, 59, 255 },
        { 22, 33, 62, 240 },
        { 40, 55, 80, 255 },
        { 51, 65, 85, 255 },
        { 248, 250, 252, 255 },
        { 148, 163, 184, 255 },
        { 0, 240, 255, 255 },
        { 0, 180, 200, 255 },
        { 0, 200, 220, 255 },
        { 255, 42, 109, 255 },
        { 255, 80, 130, 255 },
        { 120, 30, 30, 255 },
        { 255, 100, 100, 255 },
        { 0, 220, 100, 255 },
        { 51, 65, 85, 255 },
        { 80, 95, 120, 255 }
    };

    static const ThemePalette light = {
        { 245, 247, 251, 255 },
        { 255, 255, 255, 255 },
        { 255, 255, 255, 245 },
        { 235, 242, 250, 255 },
        { 196, 208, 224, 255 },
        { 24, 31, 43, 255 },
        { 92, 107, 130, 255 },
        { 0, 146, 166, 255 },
        { 0, 116, 136, 255 },
        { 0, 172, 194, 255 },
        { 214, 52, 105, 255 },
        { 235, 75, 128, 255 },
        { 210, 56, 68, 255 },
        { 232, 75, 88, 255 },
        { 0, 170, 85, 255 },
        { 219, 228, 239, 255 },
        { 205, 218, 233, 255 }
    };

    return IsLightTheme() ? light : dark;
}

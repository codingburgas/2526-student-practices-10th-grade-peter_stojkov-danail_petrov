#pragma once
#include "raylib.h"

inline Font GetUIFont() {
    static Font font{};
    static bool initialized = false;
    static bool loaded = false;

    if (!initialized) {
        initialized = true;

        const char* fontPaths[] = {
            "C:/Windows/Fonts/segoeui.ttf",
            "C:/Windows/Fonts/trebuc.ttf",
            "C:/Windows/Fonts/arial.ttf"
        };

        static int codepoints[352]{};
        static bool codepointsReady = false;
        if (!codepointsReady) {
            int index = 0;
            for (int cp = 32; cp <= 126; cp++) {
                codepoints[index++] = cp;
            }
            for (int cp = 0x0400; cp <= 0x04FF; cp++) {
                codepoints[index++] = cp;
            }
            codepointsReady = true;
        }

        for (const char* path : fontPaths) {
            if (FileExists(path)) {
                font = LoadFontEx(path, 64, codepoints, 351);
                SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
                loaded = true;
                break;
            }
        }
    }

    return loaded ? font : GetFontDefault();
}

inline int MeasureAppText(const char* text, int fontSize) {
    return (int)MeasureTextEx(GetUIFont(), text, (float)fontSize, 1.0f).x;
}

inline void DrawAppText(const char* text, int x, int y, int fontSize, Color color) {
    DrawTextEx(GetUIFont(), text, { (float)x, (float)y }, (float)fontSize, 1.0f, color);
}

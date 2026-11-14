#pragma once
#include "raylib.h"

enum class FontStyle
{
    Regular,   
    SemiBold,  
    Bold       
};

class FontManager {
public:
    FontManager() = delete;
    ~FontManager();

    static void Load();

    static void Unload();

    static const Font& Get(FontStyle style = FontStyle::Regular);

    static void Draw(const char* text, float x, float y, float size,
        Color color, FontStyle style = FontStyle::Regular,
        float spacing = 0.0f);

    static void DrawCentered(const char* text, float centerX, float y, float size,
        Color color, FontStyle style = FontStyle::Regular,
        float spacing = 0.0f);

    static Vector2 Measure(const char* text, float size,
        FontStyle style = FontStyle::Regular,
        float spacing = 0.0f);
};
//
// Created by c4ner on 11/14/2026.
//
#include "FontManager.h"
#include <string>
#include <vector>

namespace
{
    constexpr int STYLE_COUNT = 3;

    Font gFonts[STYLE_COUNT] = {};
    bool gLoaded = false;

    std::vector<int> BuildCodepoints()
    {
        std::vector<int> cp;
        for (int i = 32; i <= 126; i++)
        {
            cp.push_back(i);
        }
        for (int i = 0x0400; i <= 0x04FF; i++)
        {
            cp.push_back(i);
        }

        for (int c : { 0x00AB, 0x00BB, 0x2013, 0x2014, 0x2026, 0x2116,
            0x2022, 0x2019, 0x201C, 0x201D })
        {
            cp.push_back(c);
            return cp;
        }
    }

    std::string FontPath(const char* fileName)
    {
        return std::string(GetApplicationDirectory()) + "assets/fonts/" + fileName;
    }

    Font LoadOne(const char* path, int atlasSize, const std::vector<int>& cp)
    {
        Font f = LoadFontEx(path, atlasSize, const_cast<int*>(cp.data()), (int)cp.size());

        if (f.texture.id == 0)
        {
            TraceLog(LOG_WARNING, "FontManager: не удалось загрузить %s, используется шрифт по умолчанию", path);
            return GetFontDefault();
        }

        SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
        return f;
    }
}

FontManager::~FontManager() = default;

void FontManager::Load()
{
    if (gLoaded) return;

    const std::vector<int> cp = BuildCodepoints();

    gFonts[(int)FontStyle::Regular] = LoadOne(FontPath("Inter_18pt-Regular.ttf").c_str(), 48, cp);
    gFonts[(int)FontStyle::SemiBold] = LoadOne(FontPath("Inter_18pt-SemiBold.ttf").c_str(), 48, cp);
    gFonts[(int)FontStyle::Bold] = LoadOne(FontPath("Inter_28pt-Bold.ttf").c_str(), 64, cp);

    gLoaded = true;
}

void FontManager::Unload()
{
    if (!gLoaded) return;

    const Font def = GetFontDefault();
    for (int i = 0; i < STYLE_COUNT; i++)
    {
        if (gFonts[i].texture.id != def.texture.id)
            UnloadFont(gFonts[i]);
        gFonts[i] = {};
    }
    gLoaded = false;
}

const Font& FontManager::Get(FontStyle style)
{
    if (!gLoaded && IsWindowReady())
        Load();

    if (!gLoaded)
    {
        static Font fallback;
        fallback = GetFontDefault();
        return fallback;
    }
    return gFonts[(int)style];
}

void FontManager::Draw(const char* text, float x, float y, float size,
    Color color, FontStyle style, float spacing)
{
    DrawTextEx(Get(style), text, { x, y }, size, spacing, color);
}

void FontManager::DrawCentered(const char* text, float centerX, float y, float size,
    Color color, FontStyle style, float spacing)
{
    const Vector2 m = Measure(text, size, style, spacing);
    DrawTextEx(Get(style), text, { centerX - m.x * 0.5f, y }, size, spacing, color);
}

Vector2 FontManager::Measure(const char* text, float size,
    FontStyle style, float spacing)
{
    return MeasureTextEx(Get(style), text, size, spacing);
}
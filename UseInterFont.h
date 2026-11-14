#pragma once
#pragma once
#include "raylib.h"
#include "FontManager.h"

#define DrawText(text, x, y, size, color) \
FontManager::Draw((text), (float)(x), (float)(y), (float)(size), (color))

#define MeasureText(text, size) \
((int)FontManager::Measure((text), (float)(size)).x)
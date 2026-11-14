
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#include "ProfilerTIME.h"
#include <string>
#include "raylib.h"

ProfilerTIME::ProfilerTIME() : _currLVL(0) {
}

ProfilerTIME::~ProfilerTIME() = default;

void ProfilerTIME::BeginFrame() {
    _last_zapis = _zapis;
    _zapis.clear();
    _currLVL = 0;
}

void ProfilerTIME::AddZapis(const char* name, float time) {
    Zapis zapis;
    zapis._name = name;
    zapis._time = time;
    zapis.lvl = _currLVL;
    _zapis.push_back(zapis);
}

void ProfilerTIME::DrawZapis() const {
    constexpr int panelX = 12;
    constexpr int panelY = 12;
    constexpr int panelWidth = 440;
    constexpr int padding = 12;
    constexpr int fontSize = 18;
    constexpr int lineHeight = 26;
    constexpr int indent = 14;

    const int rows = static_cast<int>(_last_zapis.size()) + 1;
    const int panelHeight = padding * 2 + rows * lineHeight;

    DrawRectangle(panelX,panelY,panelWidth,panelHeight,Color{12, 16, 24, 230});
    int y = panelY + padding;
    DrawText(TextFormat("FPS: %d", GetFPS()),panelX + padding,y,fontSize,GREEN);

    y += lineHeight;

    for (const auto& entry : _last_zapis) {
        const int x = panelX + padding + entry.lvl * indent;
        DrawText(TextFormat("%s: %.3f ms", entry._name, entry._time),x,y,fontSize,RAYWHITE);
        y += lineHeight;
    }

}


ProfilerTIME::ScopeMarker::ScopeMarker(const char* name, ProfilerTIME &profiler) :  profiler(profiler), _name(name) {
    LARGE_INTEGER start_li;
    QueryPerformanceCounter(&start_li);
    _start = start_li.QuadPart;
    profiler._currLVL++; //+ блок коду + вложеність
}

ProfilerTIME::ScopeMarker::~ScopeMarker() {
    LARGE_INTEGER end_li;
    QueryPerformanceCounter(&end_li);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    auto duration = (static_cast<float>(end_li.QuadPart - _start) * 1000.0f / static_cast<float>(frequency.QuadPart));
    profiler.AddZapis(_name, duration);
    profiler._currLVL--;
}
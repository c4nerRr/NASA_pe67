//
// Created by c4ner on 11/11/2026.
//

#include "Init.h"

#include <algorithm>

#include "GameManager.h"
#include "MainMenu.h"
#include "raylib.h"
#include "Hub.h"

namespace {
    constexpr float maxDeltaTime = 0.05f;
}

Init::Init() = default;

Init::~Init() = default;


void Init::Run() {
    InitWindow(1920,1080, "NASA");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60); //1000 мс / 60 ≈ 16,67 мс

    auto& game = GameManager::Instanse();

    game.RegisterState(GameState::Menu, std::make_unique<MainMenu>());
    game.RegisterState(GameState::Hub, std::make_unique<Hub>());

    //MainMenu
    game.ChangeState(GameState::Menu);

    while (!WindowShouldClose() && !game.ShouldQuit()) {
        _profiler.BeginFrame();
        ProfilerTIME::ScopeMarker fullFrame("FullFrame", _profiler);

        const float dt = std::min(GetFrameTime(), maxDeltaTime);

        {
            ProfilerTIME::ScopeMarker upd("Update", _profiler);
            game.Update(dt);
        }

        {
            ProfilerTIME::ScopeMarker draw("RenderCPU", _profiler);

            BeginDrawing();
            ClearBackground(BLACK);

            game.Draw();
        }

        _profiler.DrawZapis();

        {
            ProfilerTIME::ScopeMarker present("EndDrawing/Wait", _profiler);
            EndDrawing();
        }
    }
    game.ClearStates();
    CloseWindow();
}

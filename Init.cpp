//
// Created by c4ner on 11/11/2026.
//

#include "Init.h"

#include <algorithm>

#include "GameManager.h"
#include "MainMenu.h"
#include "raylib.h"

Init::Init() = default;

Init::~Init() = default;

void Init::Run() {
    InitWindow(1920,1080, "NASA");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    auto& game = GameManager::Instanse();

    game.RegisterState(GameState::Menu, std::make_unique<MainMenu>());

    //MainMenu
    game.ChangeState(GameState::Menu);

    while (!WindowShouldClose() && !game.ShouldQuit()) {
        const float dt = std::min(GetFrameTime(), 0.05f); //TODO::убрати магічні числа

        game.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        game.Draw();

        EndDrawing();
    }
    game.ClearStates();
    CloseWindow();
}

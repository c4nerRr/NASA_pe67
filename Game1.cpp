//
// Created by c4ner on 11/11/2026.
//

#include "Game1.h"

#include "GameManager.h"
#include "InputManager.h"
#include "raylib.h"

Game1::Game1() = default;

Game1::~Game1() = default;

void Game1::Update(float dt) {
    if (InputManager::PressSPACE()) {
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }
    //TODO:логіки ігри тут
}

void Game1::Draw() const {
    const Rectangle frame{GetScreenWidth() * 0.15f,GetScreenHeight() * 0.15f,GetScreenWidth() * 0.70f,GetScreenHeight() * 0.70f};

    DrawRectangleRec(frame, MAROON);
    DrawRectangleLinesEx(frame, 4.0f, RAYWHITE);

    const int x = static_cast<int>(frame.x) + 32;
    const int y = static_cast<int>(frame.y) + 32;

    DrawText("GAME 1", x, y, 40, RAYWHITE);
    DrawText("SPACE - back to Hub", x, y + 64, 24, RAYWHITE);
}

//
// Created by c4ner on 11/11/2026.
//

#include "Game4.h"

#include "GameManager.h"
#include "InputManager.h"
#include "raylib.h"

Game4::Game4() = default;

Game4::~Game4() = default;

void Game4::Update(float dt) {
    if (InputManager::PressSPACE()) {
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }
    //TODO::логіка тут
}

void Game4::Draw() const {
    const Rectangle frame{
        GetScreenWidth() * 0.15f,
        GetScreenHeight() * 0.15f,
        GetScreenWidth() * 0.70f,
        GetScreenHeight() * 0.70f
    };

    DrawRectangleRec(frame, DARKPURPLE);
    DrawRectangleLinesEx(frame, 4.0f, RAYWHITE);

    const int x = static_cast<int>(frame.x) + 32;
    const int y = static_cast<int>(frame.y) + 32;

    DrawText("GAME 4", x, y, 40, RAYWHITE);
    DrawText("SPACE - back to Hub", x, y + 64, 24, RAYWHITE);
}

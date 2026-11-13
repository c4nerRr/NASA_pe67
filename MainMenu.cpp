//
// Created by c4ner on 11/11/2026.
//

#include "MainMenu.h"

#include "GameManager.h"
#include "InputManager.h"

MainMenu::LayOut MainMenu::GetLayOut() const { //NOLINT
    constexpr float mainWidth = 340.0f;
    constexpr float buttonHeight = 64.0f;
    constexpr float gap = 20.0f;

    constexpr float smallWidth = (mainWidth - gap) / 2.0f;

    const float centerX = GetScreenWidth() * 0.5f; //NOLINT
    const float centerY = GetScreenHeight() * 0.5f; //NOLINT

    const float left = centerX - mainWidth / 2.0f;
    const float top = centerY - buttonHeight / 2.0f;
    const float lowerY = top + buttonHeight + gap;

    return {
            {left, top, mainWidth, buttonHeight},
            {left, lowerY, smallWidth, buttonHeight},
            {left + smallWidth + gap, lowerY, smallWidth, buttonHeight}
    };
}

void MainMenu::DrawButton(Rectangle bounds, const char *text) const { //NOLINT
    const bool hovered = CheckCollisionPointRec(InputManager::MousePosition(), bounds);

    const Color normal{30, 40, 65, 255};
    const Color hover{55, 75, 110, 255};

    DrawRectangleRec(bounds, hovered ? hover : normal);
    DrawRectangleLinesEx(bounds, 2.0f, hovered ? SKYBLUE : GRAY);

    constexpr int fontSize = 22;

    const int textX = static_cast<int>(
        bounds.x + (bounds.width - MeasureText(text, fontSize)) / 2.0f //NOLINT
    );

    const int textY = static_cast<int>(
        bounds.y + (bounds.height - fontSize) / 2.0f
    );

    DrawText(text, textX, textY, fontSize, RAYWHITE);
}

MainMenu::MainMenu() = default;

MainMenu::~MainMenu() = default;

void MainMenu::Update(float dt) {
    if (!InputManager::PressLeftMouse()) {
        return;
    }
    const Vector2 mousePosition = InputManager::MousePosition();
    const LayOut layout = GetLayOut();

    auto& game = GameManager::Instanse();
    if (CheckCollisionPointRec(mousePosition, layout.play)) {
        game.ChangeState(GameState::Hub);
    }
    else if (CheckCollisionPointRec(mousePosition, layout.options)) {
        //TODO:тут сеттингс
    }
    else if (CheckCollisionPointRec(mousePosition, layout.exit)) {
        game.RequestQuit();
    }
}

void MainMenu::Draw() const {
    const LayOut layout = GetLayOut();

    DrawButton(layout.play, "PLAY");
    DrawButton(layout.options, "SETTINGS");
    DrawButton(layout.exit, "QUIT");
}


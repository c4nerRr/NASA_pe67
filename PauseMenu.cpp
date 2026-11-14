//
// Created by c4ner on 11/14/2026.
//

#include "PauseMenu.h"

#include <algorithm>

#include "GameManager.h"
#include "InputManager.h"

namespace {
    void CenteredText(const char* text,Rectangle bounds,int size,Color color) {
        DrawText(text,static_cast<int>(bounds.x +(bounds.width - MeasureText(text, size)) * 0.5f),static_cast<int>(bounds.y + (bounds.height - size) * 0.5f),size,color);
    }
}


PauseMenu::Layout PauseMenu::GetLayout() const {
    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());

    Layout layout;

    layout.scale = std::min(w / 1920.0f,h / 1080.0f);

    const float s = layout.scale;

    const float panelWidth = 520.0f * s;
    const float panelHeight = 460.0f * s;

    layout.panel = {(w - panelWidth) * 0.5f,(h - panelHeight) * 0.5f,panelWidth,panelHeight};

    const float buttonX = layout.panel.x + 60.0f * s;
    const float buttonWidth = panelWidth - 120.0f * s;
    const float buttonHeight = 64.0f * s;

    layout.resume = {buttonX,layout.panel.y + 140.0f * s,buttonWidth,buttonHeight};
    layout.menu = {buttonX,layout.panel.y + 222.0f * s,buttonWidth,buttonHeight};
    layout.quit = {buttonX,layout.panel.y + 304.0f * s,buttonWidth,buttonHeight};
    return layout;
}

void PauseMenu::DrawButton(Rectangle bounds, const char *text, float scale) const {
    const bool hovered = CheckCollisionPointRec(InputManager::MousePosition(),bounds);

    DrawRectangleRec(bounds,hovered ? Color{60, 80, 115, 255} : Color{34, 46, 70, 255});

    DrawRectangleLinesEx(bounds,2.0f,hovered ? SKYBLUE : Color{105, 125, 160, 255});

    CenteredText(text,bounds,std::max(12, static_cast<int>(26.0f * scale)),RAYWHITE);
}

PauseMenu::PauseMenu() = default;

PauseMenu::~PauseMenu() = default;

void PauseMenu::Update(float dt) {
    auto& game = GameManager::Instanse();

    if (InputManager::PressESC()) {
        game.ResumeHub();
        return;
    }

    if (!InputManager::PressLeftMouse()) {
        return;
    }

    const Vector2 mouse = InputManager::MousePosition();
    const Layout layout = GetLayout();

    if (CheckCollisionPointRec(mouse, layout.resume)) {
        game.ResumeHub();
        return;
    }

    if (CheckCollisionPointRec(mouse, layout.menu)) {
        game.ChangeState(GameState::Menu);
        return;
    }

    if (CheckCollisionPointRec(mouse, layout.quit)) {
        game.RequestQuit();
    }
}

void PauseMenu::Draw() const {
    const Layout layout = GetLayout();
    const float s = layout.scale;

    DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),Color{0, 0, 0, 165});

    DrawRectangleRec(layout.panel,Color{18, 25, 42, 250});

    DrawRectangleLinesEx(layout.panel,2.0f,Color{125, 153, 195, 255});
    CenteredText("PAUSED",{layout.panel.x,layout.panel.y + 36.0f * s,layout.panel.width,60.0f * s},std::max(16, static_cast<int>(40.0f * s)),RAYWHITE);

    DrawButton(layout.resume, "RESUME", s);
    DrawButton(layout.menu, "MAIN MENU", s);
    DrawButton(layout.quit, "QUIT GAME", s);

    CenteredText("ESC - resume",{layout.panel.x,layout.panel.y + 400.0f * s,layout.panel.width,30.0f * s},std::max(10, static_cast<int>(20.0f * s)),LIGHTGRAY);
}
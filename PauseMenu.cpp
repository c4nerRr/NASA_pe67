
#include "PauseMenu.h"

#include <algorithm>

#include "GameManager.h"
#include "InputManager.h"
#include "PauseArt.h"

namespace {
    void CenteredText(const char* text,Rectangle bounds,int size,Color color) {
        DrawText(text,static_cast<int>(bounds.x +(bounds.width - MeasureText(text, size)) * 0.5f),static_cast<int>(bounds.y + (bounds.height - size) * 0.5f),size,color);
    }
}


PauseMenu::Layout PauseMenu::GetLayout() const {
    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());
    const float scale = std::min(w / 1920.0f, h / 1080.0f);

    const auto panel = _art.Panel();
    const float panelWidth = panel.width * scale;
    const float panelHeight = panel.height * scale;

    Layout layout;
    layout.panel = {(w - panelWidth) * 0.5f, (h - panelHeight) * 0.5f, panelWidth, panelHeight};

    const auto place = [&](PauseArt::Button button, float y) {
        const auto texture = _art.GetButton(button);
        const float width = texture.width * scale;
        const float height = texture.height * scale;

        return Rectangle{layout.panel.x + (panelWidth - width) * 0.5f, layout.panel.y + y * scale, width, height};
    };

    layout.resume = place(PauseArt::Button::Resume, 200.0f);
    layout.menu = place(PauseArt::Button::Menu, 330.0f);
    layout.quit = place(PauseArt::Button::Quit, 460.0f);

    return layout;
}

void PauseMenu::DrawButton(Rectangle bounds, PauseArt::Button button) const {
    const bool hovered = CheckCollisionPointRec(InputManager::MousePosition(), bounds);
    PauseArt::Draw(_art.GetButton(button, hovered), bounds);
}


PauseMenu::PauseMenu() = default;

PauseMenu::~PauseMenu() = default;

void PauseMenu::OnEnter() {
   _art.Load();
}

void PauseMenu::Update(float dt) {
    auto& game = GameManager::Instanse();

    if (InputManager::PressESC()) {
        game.ResumeHub();
        return;
    }

    if (!InputManager::PressLeftMouse()) {
        return;
    }

    const auto mouse = InputManager::MousePosition();
    const auto layout = GetLayout();

    if (CheckCollisionPointRec(mouse, layout.resume)) {
        game.ResumeHub();
    }
    else if (CheckCollisionPointRec(mouse, layout.menu)) {
        game.ChangeState(GameState::Menu);
    }
    else if (CheckCollisionPointRec(mouse, layout.quit)) {
        game.RequestQuit();
    }
}

void PauseMenu::Draw() const {
    const auto layout = GetLayout();

    PauseArt::Draw(_art.Background(), {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())});
    PauseArt::Draw(_art.Panel(), layout.panel);

    DrawButton(layout.resume, PauseArt::Button::Resume);
    DrawButton(layout.menu, PauseArt::Button::Menu);
    DrawButton(layout.quit, PauseArt::Button::Quit);
}
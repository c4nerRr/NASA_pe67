//
// Created by c4ner on 11/11/2026.
//

#include "Game4.h"

#include "GameManager.h"
#include "InputManager.h"
#include "raylib.h"

namespace {
    constexpr int titleSize = 36;
    constexpr const char* title = "MOON GREENHOUSE";

    const float screenWidth = static_cast<float>(GetScreenWidth());
    const float screenHeight = static_cast<float>(GetScreenHeight());

    const float bedWidth = screenWidth * 0.25f;
    const float bedHeight = screenHeight * 0.17f;
    const float gap = screenHeight * 0.025f;

    const float startX =
        (screenWidth - bedWidth * 2.0f - gap) * 0.5f;

    const float startY = screenHeight * 0.25f;



    void DrawToolButton(Rectangle bounds, const char* text, bool selected) {
        const Color fill = selected ? Color{55, 105, 145, 255}: Color{35, 45, 60, 255};

        DrawRectangleRec(bounds, fill);
        DrawRectangleLinesEx(bounds, 2.0f, selected ? SKYBLUE : GRAY);

        constexpr int fontSize = 22;

        DrawText(text,static_cast<int>(bounds.x + (bounds.width - MeasureText(text, fontSize)) * 0.5f),static_cast<int>(bounds.y + (bounds.height - fontSize) * 0.5f),fontSize,RAYWHITE); //NOLINT
    }

}

void Game4::BuildLayout() {
    const auto w = static_cast<float>(GetScreenWidth());
    const auto h = static_cast<float>(GetScreenHeight());

    const float buttonW = w * 0.10f;
    const float buttonH = h * 0.075f;
    const float buttonGap = w * 0.025f;
    const float buttonsX = (w - buttonW * 2.0f - buttonGap) * 0.5f;

    _layout._waterButton = {buttonsX, h * 0.19f, buttonW, buttonH};
    _layout._lightButton = {buttonsX + buttonW + buttonGap,h * 0.19f,buttonW,buttonH};

    const float bedsWidth = w * 0.76f;
    const float gap = w * 0.035f; //NOLINT
    const float bedW = (bedsWidth - gap * (_bedCount - 1)) / _bedCount;
    const float startX = (w - bedsWidth) * 0.5f;

    for (int i = 0; i < _bedCount; ++i) {_layout._beds[i] = {
        startX + i * (bedW + gap),h * 0.34f,bedW,h * 0.43f
      };
    }
}

void Game4::ToggleTool(Tool tool) {
    _selectedTool = (_selectedTool == tool) ? Tool::None : tool;
}

void Game4::ApplyTool(Bed &bed) {
    switch (_selectedTool) {
        case Tool::Water: bed._watered = true;
            break;
        case Tool::Light: bed._light = !bed._light;
            break;
        case Tool::None:
            break;
        default:
            break;
    }
}

void Game4::ShowTheory() {
    IsHereOnce = true;
    GameManager::Instanse().ShowDialogue({
       "Welcome to the Moon greenhouse! "
       "Your mission is to help grow food for the astronauts.",

       "Plants need water, light and nutrients to grow. "
       "They also need time and the right temperature.",

       "The Moon has no air we can breathe. "
       "Our plants need a protected greenhouse with suitable air "
       "and temperature inside.",

       "Special lamps can give plants the light they need. "
       "But keeping the lamps on uses electricity.",

       "Water is precious on a Moon base. "
       "Plants need it, and astronauts need it too!",

       "A good gardener plans ahead. "
       "Use your supplies carefully and give your plants time to grow."
   });
}

void Game4::ShowRules() {
    GameManager::Instanse().ShowDialogue({
       "HOW TO PLAY: You will take care of four plant beds.",

       "Give your plants water and switch on their lamps. "
       "Both actions use resources.",

       "Press NEXT CYCLE when you are ready. "
       "Plants with enough water and light will grow.",

       "Harvest the plants when they are ready. "
       "Your goal is to collect enough food before the cycles run out!"
   });
}

void Game4::StartGame() {
    _beds = {};
    _selectedTool = Tool::None;
    _phase = Phase::Playing;
}

void Game4::UpdateGame(float dt) {
    if (InputManager::PressSPACE()) { //поки що тільки повернення в хаб
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }

    if (!InputManager::PressLeftMouse()) {
        return;
    }
    const Vector2 mouse = InputManager::MousePosition();
    if (CheckCollisionPointRec(mouse, _layout._waterButton)) {
        ToggleTool(Tool::Water);
        return;
    }

    if (CheckCollisionPointRec(mouse, _layout._lightButton)) {
        ToggleTool(Tool::Light);
        return;
    }

    if (_selectedTool == Tool::None) {
        return;
    }

    for (int i = 0; i < _bedCount; ++i) {
        if (CheckCollisionPointRec(mouse, _layout._beds[i])) {
            ApplyTool(_beds[i]);
            return;
        }
    }
}

void Game4::DrawGarden() const {
    DrawToolButton(_layout._waterButton, "WATER",_selectedTool == Tool::Water);

    DrawToolButton(_layout._lightButton, "LIGHT",_selectedTool == Tool::Light);

    for (int i = 0; i < _bedCount; ++i) {
        const Rectangle bounds = _layout._beds[i];
        const Bed& bed = _beds[i];

        DrawRectangleRec(bounds, Color{29, 53, 43, 255});
        DrawRectangleLinesEx(bounds, 2.0f, bed._light ? GOLD : DARKGREEN);

        const int x = static_cast<int>(bounds.x) + 16;
        const int y = static_cast<int>(bounds.y) + 16;
        DrawText(TextFormat("BED %d", i + 1), x, y, 26, RAYWHITE);
        DrawText(bed._watered ? "WATER: READY" : "WATER: NEEDED",x, y + 48, 20,bed._watered ? SKYBLUE : LIGHTGRAY);
        DrawText(bed._light ? "LIGHT: ON" : "LIGHT: OFF",x, y + 80, 20,bed._light ? GOLD : LIGHTGRAY);
    }
}

Game4::Game4() = default;

Game4::~Game4() = default;

void Game4::OnEnter() {
    BuildLayout();
    _phase = Phase::Theory;
    if (!IsHereOnce) {
        ShowTheory();
    }
}

void Game4::Update(float dt) {
    if (IsWindowResized()) {
        BuildLayout();
    }
    switch (_phase) {
        case Phase::Theory:
            _phase = Phase::Rules;
            ShowRules();
            return;

        case Phase::Rules:
            StartGame();
            return;

        case Phase::Playing:
            UpdateGame(dt);
            return;
    }
}

void Game4::Draw() const {
    DrawText(title,(GetScreenWidth() - MeasureText(title, titleSize)) / 2,60,titleSize,RAYWHITE);

    if (_phase == Phase::Playing) {
        DrawGarden();
        DrawText("SPACE - back to Hub",40,GetScreenHeight() - 50,22,LIGHTGRAY);
    }
}

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
    // TODO: підготувати грядки, воду енергію і розр циклів
    _phase = Phase::Playing;
}

void Game4::UpdateGame(float dt) {
    if (InputManager::PressSPACE()) { //поки що тільки повернення в хаб
        GameManager::Instanse().ChangeState(GameState::Hub);
        return;
    }
    // TODO: полів, освітлення, наступний цикл і сбор урожая
    // TODO: перевірка перемоги + програшу
}

void Game4::DrawGarden() const {
    for (int i = 0; i < 4; ++i) {
        const Rectangle bed{
            startX + (i % 2) * (bedWidth + gap),
            startY + (i / 2) * (bedHeight + gap),
            bedWidth,
            bedHeight
        };

        DrawRectangleRec(bed, Color{29, 53, 43, 255});
        DrawRectangleLinesEx(bed, 2.0f, DARKGREEN);

        DrawText(
            TextFormat("BED %d", i + 1),
            static_cast<int>(bed.x) + 20,
            static_cast<int>(bed.y) + 20,
            26,
            RAYWHITE
        );

        DrawText(
            "Ready for planting",
            static_cast<int>(bed.x) + 20,
            static_cast<int>(bed.y + bed.height) - 40,
            20,
            LIGHTGRAY
        );
    }
}

Game4::Game4() = default;

Game4::~Game4() = default;

void Game4::OnEnter() {
    _phase = Phase::Theory;
    if (!IsHereOnce) {
        ShowTheory();
    }
}

void Game4::Update(float dt) {
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

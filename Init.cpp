//
// Created by c4ner on 11/11/2026.
//

#include "Init.h"

#include <algorithm>
#include <filesystem>

#include "Game1.h"
#include "Game2.h"
#include "Game3.h"
#include "Game4.h"
#include "GameAssets.h"
#include "GameManager.h"
#include "MainMenu.h"
#include "raylib.h"
#include "Hub.h"
#include "AudioManager.h"

namespace {
    constexpr float maxDeltaTime = 0.05f;
}

Init::Init() = default;
Init::~Init() = default;


void Init::Run() {
    InitWindow(1920,1080, "NASA");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60); //1000 мс / 60 ≈ 16,67 мс

    AudioManager::Instance().Init();

    auto& game = GameManager::Instanse();

    {
        try {
            const std::filesystem::path root = GetApplicationDirectory();
            AudioManager::Instance().PlayBackgroundMusic((root / "assets" / "Nasa.ogg").string());

            const Model& moon = _assets.Load( (root / _config.moonPath).string(), _config.emissionAsAlbedo);
            std::vector<PortalModel> portals;

            for (const auto&[path, target] : _config.portals) {
                const Model& model = _assets.Load((root / path).string(),_config.emissionAsAlbedo);
                portals.push_back({&model, target});
            }
            game.RegisterState(GameState::Menu, std::make_unique<MainMenu>());
            game.RegisterState(GameState::Hub, std::make_unique<Hub>(moon, portals, _config));
            game.RegisterState(GameState::Game1, std::make_unique<Game1>());
            game.RegisterState(GameState::Game2, std::make_unique<Game2>());
            game.RegisterState(GameState::Game3, std::make_unique<Game3>());
            game.RegisterState(GameState::Game4, std::make_unique<Game4>());

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
                    ClearBackground({8,4,26,100});

                    game.Draw();
                }

                _profiler.DrawZapis();

                {
                    ProfilerTIME::ScopeMarker present("EndDrawing/Wait", _profiler);
                    EndDrawing();
                }
            }
        }
        catch(const std::exception& error) {
            TraceLog(LOG_ERROR, "%s" ,error.what());
        }
        game.ClearStates();
        _assets.Clear();
    }
    AudioManager::Instance().Close();
    CloseWindow();
}
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
#include "FontManager.h"
#include "GuideSptire.h"
#include "InputManager.h"
#include "PauseMenu.h"
#include "Setting.h"

namespace {
    constexpr float maxDeltaTime = 0.05f;
}

Init::Init() = default;
Init::~Init() = default;


void Init::Run() {
    InitWindow(1920, 1080, "NASA");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    FontManager::Load();
    GuideSprite::Load();
    AudioManager::Instance().Init();

    auto& game = GameManager::Instanse();

    {
        try {
            const std::filesystem::path root = GetApplicationDirectory();
            AudioManager::Instance().PlayBackgroundMusic((root / "assets" / "Nasa.ogg").string());

            const Model& moon = _assets.Load((root / _config.moonPath).string(), _config.emissionAsAlbedo);
            std::vector<PortalModel> portals;

            for (const auto& [path, target] : _config.portals) {
                const Model& model = _assets.Load((root / path).string(), _config.emissionAsAlbedo);
                portals.push_back({ &model, target });
            }
            game.RegisterState(GameState::Menu, std::make_unique<MainMenu>());
            game.RegisterState(GameState::Hub, std::make_unique<Hub>(moon, portals, _config));
            game.RegisterState(GameState::Game1, std::make_unique<Game1>());
            game.RegisterState(GameState::Game2, std::make_unique<Game2>());
            game.RegisterState(GameState::Game3, std::make_unique<Game3>());
            game.RegisterState(GameState::Game4, std::make_unique<Game4>());
            game.RegisterState(GameState::Pause, std::make_unique<PauseMenu>());
            game.RegisterState(GameState::Settings, std::make_unique<Setting>());

            game.ChangeState(GameState::Menu);


            Shader spaceShader = LoadShader(nullptr, (root / "assets" / "shaders" / "space2.fs").string().c_str());
            const int timeLoc = GetShaderLocation(spaceShader, "iTime");
            const int resLoc = GetShaderLocation(spaceShader, "iResolution");
            const int mouseLoc = GetShaderLocation(spaceShader, "iMouse");
            float runTime = 0.0f;

            while (!WindowShouldClose() && !game.ShouldQuit()) {
                _profiler.BeginFrame();
                ProfilerTIME::ScopeMarker fullFrame("FullFrame", _profiler);

                const float dt = std::min(GetFrameTime(), maxDeltaTime);

                {
                    ProfilerTIME::ScopeMarker upd("Update", _profiler);
                    AudioManager::Instance().Update();
                    game.Update(dt);
                }

                {
                    ProfilerTIME::ScopeMarker draw("RenderCPU", _profiler);

                    runTime += dt;
                    const float resolution[3] = { (float)GetScreenWidth(), (float)GetScreenHeight(), 1.0f };
                    const Vector2 mp = InputManager::MousePosition();
                    const float mouse[4] = { mp.x, (float)GetScreenHeight() - mp.y, 0.0f, 0.0f };

                    SetShaderValue(spaceShader, timeLoc, &runTime, SHADER_UNIFORM_FLOAT);
                    SetShaderValue(spaceShader, resLoc, resolution, SHADER_UNIFORM_VEC3);
                    SetShaderValue(spaceShader, mouseLoc, mouse, SHADER_UNIFORM_VEC4);

                    BeginDrawing();
                    ClearBackground({ 8, 4, 26, 255 });

                    BeginShaderMode(spaceShader);
                    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
                    EndShaderMode();

                    game.Draw();
                }

                if (game.GetCurrentState() != GameState::Game3) {
                    _profiler.DrawZapis();
                }

                {
                    ProfilerTIME::ScopeMarker present("EndDrawing/Wait", _profiler);
                    EndDrawing();
                }
            }
            UnloadShader(spaceShader);
        }


        catch (const std::exception& error) {
            TraceLog(LOG_ERROR, "%s", error.what());
        }
        game.ClearStates();
        _assets.Clear();
    }
    AudioManager::Instance().Close();
    FontManager::Unload();
    GuideSprite::Unload();
    CloseWindow();
}
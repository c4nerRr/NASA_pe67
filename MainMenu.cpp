#include "MainMenu.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <utility>

#include "GameManager.h"
#include "InputManager.h"

namespace {
    Texture2D LoadMenuTexture(const std::filesystem::path& path) {
        const auto name = path.string();
        Texture2D texture = LoadTexture(name.c_str());

        if (texture.id == 0) {
            throw std::runtime_error("Cannot load menu image: " + name);
        }

        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
        return texture;
    }

    void DrawImage(Texture2D texture, Rectangle bounds) {
        DrawTexturePro(
            texture,
            {
                0.0f, 0.0f,
                static_cast<float>(texture.width),
                static_cast<float>(texture.height)
            },
            bounds,
            {0.0f, 0.0f},
            0.0f,
            WHITE
        );
    }
}

MainMenu::MainMenu(MainMenuConfig config)
    : _config(std::move(config)),
      _buttons(_config.buttons.size()) {
}

MainMenu::~MainMenu() {
    if (_background.id != 0) {
        UnloadTexture(_background);
    }

    for (const auto& button : _buttons) {
        if (button.normal.id != 0) {
            UnloadTexture(button.normal);
        }

        if (button.hover.id != 0) {
            UnloadTexture(button.hover);
        }
    }
}

void MainMenu::OnEnter() {
    if (_loaded) {
        return;
    }

    const auto folder =
        std::filesystem::path(GetApplicationDirectory())
        / _config.folder;

    _background = LoadMenuTexture(folder / _config.background);

    for (std::size_t i = 0; i < _buttons.size(); ++i) {
        const auto& config = _config.buttons[i];

        _buttons[i].normal = LoadMenuTexture(folder / config.normal);
        _buttons[i].hover = LoadMenuTexture(folder / config.hover);
    }

    _loaded = true;
}

Rectangle MainMenu::GetCanvas() const {
    const auto sw = static_cast<float>(GetScreenWidth());
    const auto sh = static_cast<float>(GetScreenHeight());

    // Вписываем фон в окно, сохраняя пропорции.
    const float scale = std::min(
        sw / _background.width,
        sh / _background.height
    );

    const float width = _background.width * scale;
    const float height = _background.height * scale;

    return {
        (sw - width) * 0.5f,
        (sh - height) * 0.5f,
        width,
        height
    };
}

Rectangle MainMenu::GetButtonBounds(
    std::size_t index,
    Rectangle canvas
) const {
    const auto& config = _config.buttons[index];
    const auto texture = _buttons[index].normal;

    const float width = canvas.width * config.width;
    const float height =
        width * static_cast<float>(texture.height) / texture.width;

    return {
        canvas.x + canvas.width * config.center.x - width * 0.5f,
        canvas.y + canvas.height * config.center.y - height * 0.5f,
        width,
        height
    };
}

void MainMenu::Update(float /*dt*/) {
    if (!_loaded || !InputManager::PressLeftMouse()) {
        return;
    }

    const auto mouse = InputManager::MousePosition();
    const auto canvas = GetCanvas();

    auto& game = GameManager::Instanse();

    for (std::size_t i = 0; i < _buttons.size(); ++i) {
        const auto bounds = GetButtonBounds(i, canvas);

        if (!CheckCollisionPointRec(mouse, bounds)) {
            continue;
        }

        switch (_config.buttons[i].action) {
            case MenuAction::Play:
                game.ChangeState(GameState::Hub);
                break;

            case MenuAction::Settings:
                game.ChangeState(GameState::Settings);
                break;

            case MenuAction::Quit:
                game.RequestQuit();
                break;
        }

        return;
    }
}

void MainMenu::Draw() const {
    if (!_loaded) {
        return;
    }

    const auto canvas = GetCanvas();

    DrawImage(_background, canvas);

    for (std::size_t i = 0; i < _buttons.size(); ++i) {
        const auto bounds = GetButtonBounds(i, canvas);

        const bool hovered = CheckCollisionPointRec(
            InputManager::MousePosition(),
            bounds
        );

        const auto texture = hovered ? _buttons[i].hover : _buttons[i].normal;
        DrawImage(texture, bounds);
    }
}

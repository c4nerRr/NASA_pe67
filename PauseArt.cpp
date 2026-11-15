//
// Created by c4ner on 11/15/2026.
//

#include "PauseArt.h"
#include <filesystem>
#include <stdexcept>
#include <string>

namespace {
    constexpr auto folder = "assets/pause";

    constexpr std::array<std::array<const char*, 2>, 3> files{{
        {"apply-2.png", "apply-5.png"},
        {"apply-1.png", "apply-4.png"},
        {"apply.png", "apply-3.png"}
    }};
}

PauseArt::PauseArt() = default;

PauseArt::~PauseArt() {
    const auto release = [](const Texture2D &texture) {
        if (texture.id != 0) UnloadTexture(texture);
    };
    release(_background);
    release(_panel);

    for (const auto& button : _buttons) {
        for (const auto texture : button) release(texture);
    }
}

void PauseArt::Load() {
    if (_loaded) return;

    const auto root = std::filesystem::path(GetApplicationDirectory()) / folder;

    const auto load = [&](Texture2D& texture, const char* name) {
        if (texture.id != 0) {
            return;
        }

        const auto path = (root / name).string();
        texture = LoadTexture(path.c_str());

        if (texture.id == 0) {
            throw std::runtime_error("Cannot load pause image: " + path);
        }
        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    };

    load(_background, "pause.png");
    load(_panel, "pause_box.png");

    for (std::size_t i = 0; i < files.size(); ++i) {
        for (std::size_t state = 0; state < files[i].size(); ++state) {
            load(_buttons[i][state], files[i][state]);
        }
    }
    _loaded = true;
}

Texture2D PauseArt::Background() const {
    return _background;
}

Texture2D PauseArt::Panel() const {
    return _panel;
}

Texture2D PauseArt::GetButton(Button button, bool hovered) const {
    return _buttons.at(static_cast<std::size_t>(button)).at(hovered ? 1 : 0);
}

void PauseArt::Draw(const Texture2D &texture, const Rectangle bounds) {
    if (texture.id == 0) return;

    const Rectangle source{0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
    DrawTexturePro(texture, source, bounds, {0, 0}, 0.0f, WHITE);
}

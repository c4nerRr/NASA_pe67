#include "DialogueArt.h"

#include <filesystem>
#include <stdexcept>
#include <string>

namespace {
    constexpr const char* folder = "assets/dialogue";
    constexpr std::array<const char*, 3> files{"dialogue box.png", "button_idle.png", "button_hover.png"};
}

DialogueArt::~DialogueArt() {
    Unload();
}

void DialogueArt::Load() {
    const auto root = std::filesystem::path(GetApplicationDirectory()) / folder;
    for (std::size_t i = 0; i < files.size(); ++i) {
        if (_textures[i].id != 0) continue;
        const auto path = (root / files[i]).string();
        _textures[i] = LoadTexture(path.c_str());
        if (_textures[i].id == 0) throw std::runtime_error("Cannot load dialogue image: " + path);
        SetTextureFilter(_textures[i], TEXTURE_FILTER_BILINEAR);
    }
}

void DialogueArt::Unload() {
    for (auto& texture : _textures) {
        if (texture.id != 0) UnloadTexture(texture);
        texture = {};
    }
}

Texture2D DialogueArt::Get(Image image) const {
    return _textures.at(static_cast<std::size_t>(image));
}

void DialogueArt::Draw(Texture2D texture, Rectangle bounds) {
    if (texture.id == 0) return;
    const Rectangle source{0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
    DrawTexturePro(texture, source, bounds, {0, 0}, 0.0f, WHITE);
}

//
// Created by c4ner on 11/15/2026.
//

#include "EnergyArt.h"
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace {
    constexpr auto folder = "assets/game1";

    constexpr std::array<const char*,static_cast<std::size_t>(EnergyArt::Image::Count)> files{
        "game4.png",
        "Rectangle 81-1.png",
        "Rectangle 82.png",
        "Rectangle 81.png",
        "oxygen.png",
        "temp.png",
        "plant.png",
        "light.png"
    };
}

EnergyArt::EnergyArt() = default;

EnergyArt::~EnergyArt() {
    for (const auto texture : _textures) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
}

void EnergyArt::Load() {
    if (_loaded) return;

    const auto root = std::filesystem::path(GetApplicationDirectory()) / folder;

    for (std::size_t i = 0; i < files.size(); ++i) {
        if (_textures[i].id != 0) continue;

        const auto path = (root / files[i]).string();
        _textures[i] = LoadTexture(path.c_str());

        if (_textures[i].id == 0) {
            throw std::runtime_error( "Cannot load Game1 image: " + path);
        }
        SetTextureFilter(_textures[i],TEXTURE_FILTER_BILINEAR);
    }

    _loaded = true;
}

Texture2D EnergyArt::Get(Image image) const {
    return _textures.at(static_cast<std::size_t>(image));
}

void EnergyArt::Draw(Texture2D texture,Rectangle bounds,Color tint,float portion) {
    if (texture.id == 0) return;

    portion = std::clamp(portion, 0.0f, 1.0f);
    if (portion <= 0.0f) return;

    bounds.width *= portion;

    DrawTexturePro(texture,{0, 0,texture.width * portion,static_cast<float>(texture.height)},bounds,{0, 0},0.0f,tint);
}
//
// Created by c4ner on 11/15/2026.
//
#include <array>
#include "MissionArt.h"

#include <filesystem>

namespace {
    constexpr const char* folder = "assets/mission";

    constexpr std::array<const char*,static_cast<std::size_t>(MissionArt::Image::Count)> files{
        "box_day2.png",
        "box_day_finish2.png",
        "end_day3.png",
        "frame_day2_idle.png",
        "frame_day2_active.png"
    };
}

MissionArt::MissionArt() = default;

MissionArt::~MissionArt() {
    for (const auto texture : _textures) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    }
}

void MissionArt::Load() {
    if (_loaded) {
        return;
    }

    const auto root = std::filesystem::path(GetApplicationDirectory()) / folder;

    for (std::size_t i = 0; i < files.size(); ++i) {
        if (_textures[i].id != 0) {
            continue;
        }

        const std::string path = (root / files[i]).string();

        _textures[i] = LoadTexture(path.c_str());

        if (_textures[i].id == 0) {
            throw std::runtime_error("Cannot load mission image: " + path);
        }

        SetTextureFilter(_textures[i],TEXTURE_FILTER_BILINEAR);
    }

    _loaded = true;
}

Texture2D MissionArt::Get(Image image) const {
    return _textures.at(static_cast<std::size_t>(image));
}

void MissionArt::Draw(Texture2D texture, Rectangle bounds, Color tint) {
    if (texture.id == 0) {
        return;
    }

    const Rectangle source{0.0f,0.0f,static_cast<float>(texture.width),static_cast<float>(texture.height)};

    DrawTexturePro(texture,source,bounds,{0.0f, 0.0f},0.0f,tint);
}

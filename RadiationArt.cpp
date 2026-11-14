//
// Created by c4ner on 11/14/2026.
//

#include "RadiationArt.h"

#include <filesystem>

namespace {
    constexpr const char* folder = "assets/game3";
    constexpr std::array<const char*, RadiationBase::ModuleCount> names{"greenhouse", "habitat", "lab"};
    constexpr std::array<const char*, 3> suffixes{"", "_both", "_bad"};
}

RadiationArt::~RadiationArt() {
    const auto release = [](Texture2D texture) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    };

    release(_background);
    release(_switch);
    release(_rules);

    for (const auto& building : _buildings) {
        for (const auto texture : building) {
            release(texture);
        }
    }
}

RadiationArt::RadiationArt() = default;

void RadiationArt::Load() {
    if (_loaded) return;

    const auto root = std::filesystem::path(GetApplicationDirectory()) / folder;

    const auto load = [&](Texture2D& texture,const std::string& name) {
        if (texture.id != 0) return;

        const auto path = (root / name).string();
        texture = LoadTexture(path.c_str());

        if (texture.id == 0) {
            throw std::runtime_error("Cannot load Game3 image: " + path);
        }
        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    };

    load(_background, "game3.png");
    load(_switch, "switch.png");
    load(_rules, "rules.png");

    for (std::size_t i = 0; i < names.size(); ++i) {
        for (std::size_t j = 0; j < suffixes.size(); ++j) {
            load(_buildings[i][j],std::string(names[i]) + suffixes[j] + ".png");
        }
    }

    _loaded = true;
}

Texture2D RadiationArt::Background() const { return _background; }

Texture2D RadiationArt::Switch() const { return _switch; }

Texture2D RadiationArt::Rules() const { return _rules; }

Texture2D RadiationArt::Building(RadiationBase::Module module, State state) const {
    return _buildings.at(static_cast<std::size_t>(module)).at(static_cast<std::size_t>(state));
}

Rectangle RadiationArt::Fit(Texture2D texture, Rectangle bounds) {
    if (texture.width <= 0 || texture.height <= 0) {
        return bounds;
    }

    const float scale = std::min(bounds.width / texture.width,bounds.height / texture.height);

    const float width = texture.width * scale;
    const float height = texture.height * scale;

    return {bounds.x + (bounds.width - width) * 0.5f,bounds.y + (bounds.height - height) * 0.5f,width,height};
}

void RadiationArt::Draw(Texture2D texture, Rectangle bounds, Color tint) {
    if (texture.id == 0) return;

    DrawTexturePro(texture,{0, 0,static_cast<float>(texture.width),static_cast<float>(texture.height)},bounds,{0, 0},0.0f,tint);
}

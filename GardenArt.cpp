//
// Created by c4ner on 11/14/2026.
//

#include "GardenArt.h"

#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <string>

GardenArt::~GardenArt() {
    const auto release = [](Texture2D texture) {
        if (texture.id != 0) {
            UnloadTexture(texture);
        }
    };

    release(_background);
    release(_soil);

    for (const auto texture : _icons) {
        release(texture);
    }

    for (const auto& color : _flowers) {
        for (const auto texture : color) {
            release(texture);
        }
    }
}

void GardenArt::Load() {
    if (_loaded) return;

    const auto root = std::filesystem::path(GetApplicationDirectory()) / GardenDesign::folder;

    const auto load = [&]( Texture2D& texture,const std::string& name) {
        if (texture.id != 0) return;

        const auto path = (root / name).string();
        texture = LoadTexture(path.c_str());

        if (texture.id == 0) {
            throw std::runtime_error("Cannot load greenhouse image: " + path);
        }

        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    };

    load(_background, GardenDesign::background);
    load(_soil, GardenDesign::soil);

    for (std::size_t i = 0; i < _icons.size(); ++i) {
        load(_icons[i], GardenDesign::icons[i]);
    }

    for (std::size_t color = 0; color < _flowers.size(); ++color) {
        for (std::size_t stage = 0;stage < _flowers[color].size();++stage) {
            load(_flowers[color][stage],std::string("flower_")+ GardenDesign::colors[color]+ std::to_string(GardenDesign::stages[stage])+ ".png");
             }
    }

    _loaded = true;
}

Texture2D GardenArt::Background() const {
    return _background;
}

Texture2D GardenArt::Soil() const {
    return _soil;
}

Texture2D GardenArt::Water(bool available) const {
    return _icons[available ? 0 : 1];
}

Texture2D GardenArt::Light(bool on) const {
    return _icons[on ? 2 : 3];
}

Texture2D GardenArt::Flower(std::size_t color, std::size_t stage) const {
    return _flowers.at(color).at(stage);
}

void GardenArt::DrawFill(Texture2D texture, Rectangle bounds) {
    if (texture.id == 0) return;

    DrawTexturePro(texture,{0, 0,static_cast<float>(texture.width),static_cast<float>(texture.height)},bounds,{0, 0},0.0f,WHITE);

}

void GardenArt::DrawFit(Texture2D texture, Rectangle bounds) {
    if (texture.id == 0) return;

    const float scale = std::min(bounds.width / texture.width,bounds.height / texture.height);

    const float width = texture.width * scale;
    const float height = texture.height * scale;

    DrawFill(texture, {bounds.x + (bounds.width - width) * 0.5f,bounds.y + (bounds.height - height) * 0.5f,width,height});
}

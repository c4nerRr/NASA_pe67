//
// Created by c4ner on 11/14/2026.
//
#include "GardenArt.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace {
    constexpr std::array<
        std::array<const char*, 2>,
        static_cast<std::size_t>(GardenArt::Button::Count)
    > buttonFiles{{
        {"water_idle.png", "water_hover.png"},
        {"light_idle.png", "light_hover.png"},
        {"endday_idle.png", "endday_hover.png"},
        {"rules_idle.png", "rules_hover.png"}
    }};
}

GardenArt::~GardenArt() {
    const auto release = [](const Texture2D &texture) {
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

    for (const auto& button : _buttons) {
        for (const auto texture : button) {
            release(texture);
        }
    }
}

void GardenArt::Load() {
    if (_loaded) {
        return;
    }

    const auto root =
        std::filesystem::path(GetApplicationDirectory()) /
        GardenDesign::folder;

    const auto load = [&](
        Texture2D& texture,
        const std::string& name
    ) {
        if (texture.id != 0) {
            return;
        }

        const auto path = (root / name).string();
        texture = LoadTexture(path.c_str());

        if (texture.id == 0) {
            throw std::runtime_error(
                "Cannot load greenhouse image: " + path
            );
        }

        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    };

    load(_background, GardenDesign::background);
    load(_soil, GardenDesign::soil);

    for (std::size_t i = 0; i < _icons.size(); ++i) {
        load(_icons[i], GardenDesign::icons[i]);
    }

    for (std::size_t color = 0; color < _flowers.size(); ++color) {
        for (
            std::size_t stage = 0;
            stage < _flowers[color].size();
            ++stage
        ) {
            load(
                _flowers[color][stage],
                std::string("flower_") +
                GardenDesign::colors[color] +
                std::to_string(GardenDesign::stages[stage]) +
                ".png"
            );
        }
    }

    for (std::size_t i = 0; i < _buttons.size(); ++i) {
        for (std::size_t state = 0; state < 2; ++state) {
            load(_buttons[i][state], buttonFiles[i][state]);
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

Texture2D GardenArt::Flower(
    std::size_t color,
    std::size_t stage
) const {
    return _flowers.at(color).at(stage);
}

Texture2D GardenArt::GetButton(
    Button button,
    bool hovered
) const {
    return _buttons.at(
        static_cast<std::size_t>(button)
    ).at(hovered ? 1 : 0);
}

Rectangle GardenArt::Fit(
    Texture2D texture,
    Rectangle bounds
) {
    if (texture.width <= 0 || texture.height <= 0) {
        return bounds;
    }

    const float scale = std::min(
        bounds.width / texture.width,
        bounds.height / texture.height
    );

    const float width = texture.width * scale;
    const float height = texture.height * scale;

    return {
        bounds.x + (bounds.width - width) * 0.5f,
        bounds.y + (bounds.height - height) * 0.5f,
        width,
        height
    };
}

void GardenArt::DrawFill(
    Texture2D texture,
    Rectangle bounds
) {
    if (texture.id == 0) {
        return;
    }

    DrawTexturePro(
        texture,
        {
            0.0f,
            0.0f,
            static_cast<float>(texture.width),
            static_cast<float>(texture.height)
        },
        bounds,
        {0.0f, 0.0f},
        0.0f,
        WHITE
    );
}

void GardenArt::DrawFit(
    Texture2D texture,
    Rectangle bounds
) {
    DrawFill(texture, Fit(texture, bounds));
}

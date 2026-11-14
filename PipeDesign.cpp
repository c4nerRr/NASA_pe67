//
// Created by c4ner on 11/15/2026.
//
#include <array>
#include <filesystem>


namespace {
    constexpr auto folder = "assets/game2";
    constexpr std::array<const char*, 3> suffixes{"", "_dirty", "_water"};
    constexpr std::array<int, 4> directions{4, 2, 3, 1};
}

#include "PipeDesign.h"

PipeArt::PipeArt() = default;

PipeArt::~PipeArt() {
    const auto release = [](const Texture2D &texture) {
        if (texture.id != 0) UnloadTexture(texture);
    };

    release(_background);
    release(_board);

    for (const auto& shape : _pipes)
        for (const auto texture : shape) {
            release(texture);
        }

    for (const auto texture : _sources) {
        release(texture);
    }

    for (const auto& state : _reservoirs)
        for (const auto texture : state) {
            release(texture);
        }

    for (const auto texture : _filters) {
        release(texture);
    }
}

void PipeArt::Load() {
    if (_loaded) {
        return;
    }
    const auto root = std::filesystem::path(GetApplicationDirectory()) / folder;

    const auto load = [&](Texture2D& texture, const std::string& name) {
        if (texture.id != 0) {
            return;
        }

        const auto path = (root / name).string();
        texture = LoadTexture(path.c_str());

        if (texture.id == 0) {
            throw std::runtime_error("Cannot load Game2 image: " + path);
        }

        SetTextureFilter(texture, TEXTURE_FILTER_BILINEAR);
    };

    load(_background, "game1_bg.png");
    load(_board, "box.png");
    load(_filters[0], "filter_off.png");
    load(_filters[1], "filter_on2.png");

    for (std::size_t shape = 0; shape < _pipes.size(); ++shape) {
        for (std::size_t flow = 0; flow < suffixes.size(); ++flow) {
            load(_pipes[shape][flow], std::to_string(shape + 1) + "pipe" + suffixes[flow] + ".png");
        }
    }

    for (std::size_t i = 0; i < directions.size(); ++i) {
        const auto ending = std::to_string(directions[i]) + ".png";
        load(_sources[i], "start" + ending);
        load(_reservoirs[0][i], "end_empty" + ending);
        load(_reservoirs[1][i], "end" + ending);
    }
    _loaded = true;
}

Texture2D PipeArt::Background() const {
    return _background;
}

Texture2D PipeArt::Board() const {
    return _board;
}

Texture2D PipeArt::Tube(const bool corner, Flow flow) const {
    return _pipes.at(corner ? 1 : 0).at(static_cast<std::size_t>(flow));
}

Texture2D PipeArt::Source(int direction) const {
    return _sources.at(static_cast<std::size_t>(direction));
}

Texture2D PipeArt::Reservoir(int direction, bool filled) const {
    return _reservoirs.at(filled ? 1 : 0).at(static_cast<std::size_t>(direction));
}

void PipeArt::DrawFilter(Rectangle cell, bool active) const {
    const auto texture = _filters[active ? 1 : 0];
    if (texture.id == 0) return;

    const Rectangle target{
        cell.x + cell.width * 0.2f,
        cell.y + cell.height * 0.2f,
        cell.width * 0.6f,
        cell.height * 0.6f
    };

    DrawTexturePro(texture, PipeDesign::filterCrop, target, {0, 0}, 0.0f, WHITE);
}

void PipeArt::Draw(Texture2D texture, Rectangle bounds, float angle) {
    if (texture.id == 0) {
        return;
    }

    const Rectangle source{0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height)};
    const Vector2 origin{bounds.width * 0.5f, bounds.height * 0.5f};

    bounds.x += origin.x;
    bounds.y += origin.y;

    DrawTexturePro(texture, source, bounds, origin, angle, WHITE);
}
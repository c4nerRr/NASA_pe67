//
// Created by c4ner on 11/15/2026.
//
#pragma once

#include <array>
#include <cstddef>

#include "raylib.h"

class MissionArt {
public:
    enum class Image {
        Panel,
        Confirmation,
        Result,
        FrameIdle,
        FrameActive,
        Count
    };

    MissionArt();
    ~MissionArt();

    MissionArt(const MissionArt&) = delete;
    MissionArt& operator=(const MissionArt&) = delete;

    void Load();

    [[nodiscard]] Texture2D Get(Image image) const;

    static void Draw(Texture2D texture,Rectangle bounds,Color tint = WHITE);

private:
    std::array<Texture2D,static_cast<std::size_t>(Image::Count)> _textures{};

    bool _loaded = false;
};
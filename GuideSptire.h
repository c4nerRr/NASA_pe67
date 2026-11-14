#pragma once

#include <algorithm>
#include <filesystem>

#include "raylib.h"

namespace GuideSprite {
    inline Texture2D idle{};
    inline Texture2D talk{};

    inline void Unload() {
        if (idle.id != 0) UnloadTexture(idle);
        if (talk.id != 0) UnloadTexture(talk);

        idle = {};
        talk = {};
    }

    inline void Load() {
        Unload();

        const auto folder =
            std::filesystem::path(GetApplicationDirectory())
            / "assets" / "guide";

        idle = LoadTexture((folder / "idle.png").string().c_str());

        const auto talkPath = (folder / "talk.png").string();

        if (FileExists(talkPath.c_str())) {
            talk = LoadTexture(talkPath.c_str());
        }

        if (idle.id != 0) {
            SetTextureFilter(idle, TEXTURE_FILTER_BILINEAR);
        }

        if (talk.id != 0) {
            SetTextureFilter(talk, TEXTURE_FILTER_BILINEAR);
        }
    }

    inline void Draw(Rectangle bounds, bool speaking) {
        const Texture2D texture =
            speaking && talk.id != 0 ? talk : idle;

        if (texture.id == 0 ||
            texture.width <= 0 || texture.height <= 0) {
            return;
        }

        const float scale = std::min(
            bounds.width / static_cast<float>(texture.width),
            bounds.height / static_cast<float>(texture.height)
        );

        const float width = texture.width * scale;
        const float height = texture.height * scale;

        const Rectangle destination{
            bounds.x + (bounds.width - width) * 0.5f,
            bounds.y + bounds.height - height,
            width,
            height
        };

        DrawTexturePro(
            texture,
            {0.0f, 0.0f,
             static_cast<float>(texture.width),
             static_cast<float>(texture.height)},
            destination,
            {0.0f, 0.0f},
            0.0f,
            WHITE
        );
    }
}
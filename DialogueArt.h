#pragma once

#include <array>
#include "raylib.h"

class DialogueArt {
public:
    enum class Image { Panel, SkipIdle, SkipHover, Count };

    DialogueArt() = default;
    ~DialogueArt();
    DialogueArt(const DialogueArt&) = delete;
    DialogueArt& operator=(const DialogueArt&) = delete;

    void Load();
    void Unload();
    [[nodiscard]] Texture2D Get(Image image) const;
    static void Draw(Texture2D texture, Rectangle bounds);

private:
    std::array<Texture2D, static_cast<std::size_t>(Image::Count)> _textures{};
};

//
// Created by c4ner on 11/15/2026.
//

#ifndef NASA_PAUSEART_H
#define NASA_PAUSEART_H
#include <array>
#include "raylib.h"


class PauseArt {
public:
    enum class Button { Resume, Menu, Quit };

    PauseArt();
    ~PauseArt();

    PauseArt(const PauseArt&) = delete;
    PauseArt& operator=(const PauseArt&) = delete;

    void Load();

    [[nodiscard]] Texture2D Background() const;
    [[nodiscard]] Texture2D Panel() const;
    [[nodiscard]] Texture2D GetButton(Button button, bool hovered = false) const;

    static void Draw(const Texture2D &texture, Rectangle bounds);

private:
    Texture2D _background{};
    Texture2D _panel{};
    std::array<std::array<Texture2D, 2>, 3> _buttons{};
    bool _loaded = false;
};


#endif //NASA_PAUSEART_H
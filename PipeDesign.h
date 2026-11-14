//
// Created by c4ner on 11/15/2026.
//

#ifndef NASA_PIPEDESIGN_H
#define NASA_PIPEDESIGN_H
#include <array>

namespace PipeDesign {
    inline constexpr Vector2 boardSize{900, 1000};
    inline constexpr Rectangle gridArea{60, 158, 780, 780};
    inline constexpr Rectangle filterCrop{26, 26, 78, 78};
}

class PipeArt {
public:
    enum class Flow { Dry, Dirty, Clean };

    PipeArt();
    ~PipeArt();

    PipeArt(const PipeArt&) = delete;
    PipeArt& operator=(const PipeArt&) = delete;

    void Load();

    [[nodiscard]] Texture2D Background() const;
    [[nodiscard]] Texture2D Board() const;
    [[nodiscard]] Texture2D Tube(bool corner, Flow flow) const;
    [[nodiscard]] Texture2D Source(int direction) const;
    [[nodiscard]] Texture2D Reservoir(int direction, bool filled) const;

    void DrawFilter(Rectangle cell, bool active) const;
    static void Draw(Texture2D texture, Rectangle bounds, float angle = 0.0f);

private:
    Texture2D _background{};
    Texture2D _board{};

    std::array<std::array<Texture2D, 3>, 2> _pipes{};
    std::array<Texture2D, 4> _sources{};
    std::array<std::array<Texture2D, 4>, 2> _reservoirs{};
    std::array<Texture2D, 2> _filters{};

    bool _loaded = false;
};


#endif //NASA_PIPEDESIGN_H
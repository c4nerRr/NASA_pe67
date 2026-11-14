//
// Created by c4ner on 11/14/2026.
//

#ifndef NASA_GARDENART_H
#define NASA_GARDENART_H
#include <array>
#include "raylib.h"


namespace GardenDesign {
    inline constexpr const char* folder = "assets/game4";
    inline constexpr const char* background = "game2_bg.png";
    inline constexpr const char* soil = "Frame 5.png";

    inline constexpr std::array<const char*, 5> colors{
        "blue", "red", "orange", "white", "purple"
    };
    inline constexpr std::array<int, 4> stages{2, 3, 4, 1};

    inline constexpr std::array<const char*, 4> icons{
        "water.png", "no_water.png",
        "light.png", "no_light.png"
    };
}

class GardenArt {
    Texture2D _background{};
    Texture2D _soil{};

    std::array<Texture2D, GardenDesign::icons.size()> _icons{};
    std::array<std::array<Texture2D, GardenDesign::stages.size()>,GardenDesign::colors.size()> _flowers{};

    bool _loaded = false;

    public:
    GardenArt() = default;
    ~GardenArt();
    GardenArt(const GardenArt&) = delete;
    GardenArt& operator=(const GardenArt&) = delete;

    void Load();
    [[nodiscard]] Texture2D Background() const;
    [[nodiscard]] Texture2D Soil() const;

    [[nodiscard]] Texture2D Water(bool available) const;
    [[nodiscard]] Texture2D Light(bool on) const;

    [[nodiscard]] Texture2D Flower(std::size_t color, std::size_t stage) const;

    static void DrawFill(Texture2D texture, Rectangle bounds);
    static void DrawFit(Texture2D texture, Rectangle bounds);

};

#endif //NASA_GARDENART_H
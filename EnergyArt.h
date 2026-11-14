//
// Created by c4ner on 11/15/2026.
//

#ifndef NASA_ENERGYART_H
#define NASA_ENERGYART_H
#include <array>
#include <cstddef>


class EnergyArt {
public:
    enum class Image {
        Background,
        GoodBar,
        BadBar,
        Handle,
        Oxygen,
        Heating,
        Greenhouse,
        Lighting,
        Count
    };

    EnergyArt();
    ~EnergyArt();

    EnergyArt(const EnergyArt&) = delete;
    EnergyArt& operator=(const EnergyArt&) = delete;

    void Load();
    [[nodiscard]] Texture2D Get(Image image) const;

    static void Draw(Texture2D texture,Rectangle bounds,Color tint = WHITE,float portion = 1.0f);

private:
    std::array<Texture2D,static_cast<std::size_t>(Image::Count)> _textures{};

    bool _loaded = false;
};


#endif //NASA_ENERGYART_H
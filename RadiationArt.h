//
// Created by c4ner on 11/14/2026.
//

#ifndef NASA_RADIATIONART_H
#define NASA_RADIATIONART_H
#include <array>
#include "RadiationBase.h"

class RadiationArt {
public:
    enum class State {
        Normal,
        Powered,
        Damaged
    };

    RadiationArt();
    ~RadiationArt();

    RadiationArt(const RadiationArt&) = delete;
    RadiationArt& operator=(const RadiationArt&) = delete;

    void Load();

    [[nodiscard]] Texture2D Background() const;

    [[nodiscard]] Texture2D Switch() const;

    [[nodiscard]] Texture2D Rules() const;

    [[nodiscard]] Texture2D Building(RadiationBase::Module module,State state) const;

    static Rectangle Fit(Texture2D texture, Rectangle bounds);

    static void Draw(Texture2D texture,Rectangle bounds,Color tint = WHITE);

private:
    Texture2D _background{};
    Texture2D _switch{};
    Texture2D _rules{};

    std::array<std::array<Texture2D, 3>,RadiationBase::ModuleCount> _buildings{};

    bool _loaded = false;
};


#endif //NASA_RADIATIONART_H
//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_HUB_H
#define NASA_HUB_H
#include <array>

#include "HubPortal.h"
#include "IState.h"
#include "Objects3D.h"


class Hub : public IState {
    Objects3D _objects3D;
    Camera3D _camera{};
    std::array<HubPortal, 4> _portals{};

public:
    Hub();
    ~Hub() override;

    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_HUB_H
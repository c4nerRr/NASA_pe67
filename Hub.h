//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_HUB_H
#define NASA_HUB_H
#include <array>
#include <vector>

#include "HubConfig.h"
#include "HubPortal.h"
#include "IState.h"
#include "Objects3D.h"

struct PortalModel {
    const Model* model;
    GameState target;
};

class Hub : public IState {
    Matrix _sceneTransform;
    Objects3D _objects3D;
    Camera3D _camera{};

    std::vector<HubPortal> _portals;

    bool _doubleSided;

public:
    Hub(const Model& moon, const std::vector<PortalModel>& port, const HubConfig& config);
    ~Hub() override;

    void Update(float dt) override;
    void Draw() const override;
};


#endif //NASA_HUB_H
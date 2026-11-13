//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_HUB_H
#define NASA_HUB_H

#include <vector>

#include "HubConfig.h"
#include "HubOutline.h"
#include "HubPortal.h"
#include "IState.h"
#include "Objects3D.h"
#include "ScaneLight.h"

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
    bool _introShown = false;

    int _hoveredPortal = -1;
    HubOutline _outline;
    ScaneLight _lighting;

public:
    Hub(const Model& moon, const std::vector<PortalModel>& port, const HubConfig& config);
    ~Hub() override;

    void Update(float dt) override;
    void Draw() const override;

    void OnEnter() override;
};

#endif //NASA_HUB_H
//
// Created by c4ner on 11/11/2026.
//

#include "Hub.h"

#include "raymath.h"
#include <algorithm>
#include <stdexcept>
#include <raymath.h>
#include <rlgl.h>


//         Hub ->
//Objects3D && HubPortal

namespace {
    Matrix MakeSceneTransform(const Model& moon,const HubConfig& config) {
        const BoundingBox bounds = GetModelBoundingBox(moon);

        const Vector3 center = Vector3Scale( Vector3Add(bounds.min, bounds.max), 0.5f);

        const Vector3 size = Vector3Subtract(bounds.max, bounds.min);

        const float diameter = std::max({size.x, size.y, size.z});

        if (diameter <= 0 || config.moonRadius <= 0) {
            throw std::runtime_error("Invalid moon size");
        }

        const float scale = config.moonRadius * 2.0f / diameter;

        Matrix result = MatrixTranslate(-center.x, -center.y, -center.z);

        result = MatrixMultiply(result, MatrixRotateY(config.sceneYaw * DEG2RAD));

        result = MatrixMultiply(result,MatrixScale(scale, scale, scale));

        return MatrixMultiply(result,MatrixTranslate(config.moonCenter.x,config.moonCenter.y,config.moonCenter.z));
    }
}

Hub::Hub( const Model& moon, const std::vector<PortalModel>& port, const HubConfig& config)
    : _sceneTransform(MakeSceneTransform(moon, config)),
      _objects3D(moon, _sceneTransform),
      _camera(config.camera),
      _doubleSided(config.doubleSided) {

    _portals.reserve(port.size());

    for (const auto& portal : port) {
        _portals.emplace_back(
            *portal.model,
            _sceneTransform,
            portal.target
        );
    }
}



Hub::~Hub() = default;

void Hub::Update(float dt) {
    //TODO:Мишка тут(вибір бази точніше тут)

}

void Hub::Draw() const {
    BeginMode3D(_camera);
    if (_doubleSided) {
        rlDisableBackfaceCulling();
    }
    _objects3D.Draw();
    for (const HubPortal& portal : _portals) {
        portal.Draw();
    }
    if (_doubleSided) {
        rlEnableBackfaceCulling();
    }
    EndMode3D();
}



#include "Hub.h"

#include "raymath.h"
#include <algorithm>
#include <stdexcept>
#include <raymath.h>
#include <rlgl.h>

#include "InputManager.h"

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
    std::vector<GameState> MissionStations(const std::vector<PortalModel>& portals) {
        std::vector<GameState> stations;
        stations.reserve(portals.size());

        for (const auto& portal : portals) {
            stations.push_back(portal.target);
        }
        return stations;
    }
}

Hub::Hub( const Model& moon, const std::vector<PortalModel>& port, const HubConfig& config): _sceneTransform(MakeSceneTransform(moon, config)),_objects3D(moon, _sceneTransform),_camera(config.camera),_doubleSided(config.doubleSided), _lighting(config.lighting),  _mission(MissionStations(port)) {
    _portals.reserve(port.size());

    for (const auto& portal : port) {
        _portals.emplace_back(*portal.model,_sceneTransform,portal.target);
    }
}



Hub::~Hub() = default;

void Hub::Update(float dt) {
    _hoveredPortal = -1;
    if (_mission.Update()) {
        return;
    }
    const Ray ray = GetScreenToWorldRay(InputManager::MousePosition(),_camera);

    float nearestDistance = std::numeric_limits<float>::infinity();
    int nearestPortal = -1;
    for (int i = 0; i < static_cast<int>(_portals.size()); ++i) {
        const float distance = _portals[i].GetRayHitDistance(ray);

        if (distance < nearestDistance) {
            nearestDistance = distance;
            nearestPortal = i;
        }
    }

    if (nearestPortal < 0) {
        return;
    }
    if (_objects3D.GetRayHitDistance(ray) <= nearestDistance) {
        return;
    }

    auto& game = GameManager::Instanse();
    const GameState target = _portals[nearestPortal]._targetState;

    if (!game.CanEnterState(target)) {
        return;
    }
    _hoveredPortal = nearestPortal;

    _outline.Resize(GetScreenWidth(), GetScreenHeight());

    if (InputManager::PressLeftMouse()) {
        _hoveredPortal = -1;
        game.ChangeState(target);
        return;
    }

}

void Hub::Draw() const {
    BeginMode3D(_camera);

    if (_doubleSided) {
        rlDisableBackfaceCulling();
    }
    const Shader shader = _lighting.GetShader();

    _objects3D.DrawWithShader(shader);

    for (const HubPortal& portal : _portals) {
        portal.Draw(shader);
    }

    if (_doubleSided) {
        rlEnableBackfaceCulling();
    }

    EndMode3D();

    if (_hoveredPortal >= 0) {
        _outline.Draw(_camera,_objects3D,_portals,_hoveredPortal,_doubleSided);
    }
    _mission.Draw();
}

void Hub::OnEnter() {
    _hoveredPortal = -1;

   // if (_introShown) {
     //   return;
    //}

    _introShown = GameManager::Instanse().ShowDialogue({
         "Welcome to the Moon, explorer! "
         "I will be your mission guide.",

         "Day 1: repair every station by completing its challenge. "
         "Choose any station to begin.",

         "Click the guide at the bottom-right whenever "
         "you need your next step.",

         "After repairs, click the guide to begin Day 2. "
         "Manage power, food and water until the supply ship arrives. "
         "The guide will wait for your click before moving on."
     });
}

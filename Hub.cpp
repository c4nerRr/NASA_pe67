//
// Created by c4ner on 11/11/2026.
//

#include "Hub.h"

Hub::Hub() { //TODO: Камеру не позоритись і перенести в геймкамера клас.
    _camera.position = {0.0f, 3.0f, 9.0f};
    _camera.target = {0.0f, -1.0f, 0.0f};
    _camera.up = {0.0f, 1.0f, 0.0f};
    _camera.fovy = 45.0f;
    _camera.projection = CAMERA_PERSPECTIVE;

    const auto makePortal = [this](Vector3 direction,GameState target,Color color) {
      HubPortal portal; //TODO: задуматись нашо іменно тут ініціалізуеш.
        portal.position = _objects3D.GetSurfacePoint(direction);
        portal.targetState = target;
        portal.color = color;

        constexpr float heightOffset = 0.35f;
        portal.position.y += portal.size.y * heightOffset;

        return portal;
    };

    _portals = {{
        makePortal({0.0f, 1.0f, -0.30f},GameState::Game1,SKYBLUE),
        makePortal({-0.30f, 1.0f, 0.0f},GameState::Game2,PURPLE),
        makePortal({0.30f, 1.0f, 0.0f},GameState::Game3,ORANGE),
        makePortal({0.0f, 1.0f, 0.30f},GameState::Game4,GREEN
        )}};
}

Hub::~Hub() = default;

void Hub::Update(float dt) {
    //TODO:Мишка тут

}

void Hub::Draw() const {
    BeginMode3D(_camera);
    _objects3D.Draw();
    for (const HubPortal& portal : _portals) {
        portal.Draw();
    }
    EndMode3D();
}

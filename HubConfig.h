

#ifndef NASA_HUBCONFIG_H
#define NASA_HUBCONFIG_H
#include <string>
#include <vector>

#include "GameManager.h"
#include "raylib.h"
#include "LightConfig.h"

struct PortalConfig {
    std::string path;
    GameState target;
};

struct HubConfig {
    LightConfig lighting{};

    std::string moonPath = "assets/models/moon.glb";

    std::vector<PortalConfig> portals{
        {"assets/models/base1.glb", GameState::Game1},
        {"assets/models/base2.glb", GameState::Game2},
        {"assets/models/base3.glb", GameState::Game3},
        {"assets/models/base4.glb", GameState::Game4}
    };
    Vector3 moonCenter{0.0f, -10.0f, 0.0f};
    float moonRadius = 10.0f;
    float sceneYaw = 90.0f; //шоб була вся композа в розмірі

    Camera3D camera{ {0,4,11}, {0,-1,0}, {0,1,0}, 45, CAMERA_PERSPECTIVE };

    bool emissionAsAlbedo = true;
    bool doubleSided = true;
};

#endif //NASA_HUBCONFIG_H
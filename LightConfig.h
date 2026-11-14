
#ifndef NASA_LIGHTCONFIG_H
#define NASA_LIGHTCONFIG_H
#include <string>

#include "raylib.h"


class LightConfig {

    public:
    std::string vertexShaderPath = "assets/shaders/scene_light.vs";
    std::string fragmentShaderPath = "assets/shaders/scene_light.fs";

    //Направлення ВІД сцени до істочника
    //азімут 0: істочник йде від З
    //отріц. азімут: смєщєніе по Х
    float sunAzimuthDegrees = -35.0f;
    float sunElevationDegrees = 50.0f;

    Color sunColor{255, 230, 210, 255};
    float sunIntensity = 0.40f;

    Color ambientColor{214, 220, 244, 255};
    float ambientIntensity = 0.70f;
};


#endif //NASA_LIGHTCONFIG_H
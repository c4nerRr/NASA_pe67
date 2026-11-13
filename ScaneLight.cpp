//
// Created by c4ner on 11/13/2026.
//

#include "ScaneLight.h"

#include <cmath>
#include <filesystem>
#include <stdexcept>

namespace {
    Vector3 MakeLightColor(Color color, float intensity) {
        return {color.r / 255.0f * intensity,color.g / 255.0f * intensity,color.b / 255.0f * intensity}; //NOLINT
    }
}


ScaneLight::ScaneLight(const LightConfig &config) {
    if (config.sunIntensity < 0.0f || config.ambientIntensity < 0.0f) {
        throw std::runtime_error("Light intensity cannot be negative");
    }

    const std::filesystem::path root = GetApplicationDirectory();

    const std::string vertexPath = (root / config.vertexShaderPath).string();

    const std::string fragmentPath = (root / config.fragmentShaderPath).string();

    _shader = LoadShader(vertexPath.c_str(), fragmentPath.c_str());

    const int directionLoc = GetShaderLocation(_shader, "directionToLight");

    const int sunLoc = GetShaderLocation(_shader, "sunColor");

    const int ambientLoc = GetShaderLocation(_shader, "ambientColor");

    if (directionLoc < 0 || sunLoc < 0 || ambientLoc < 0) {
        UnloadShader(_shader);
        _shader = {};

        throw std::runtime_error("Failed to load scene lighting shader");
    }


    const float azimuth = config.sunAzimuthDegrees * DEG2RAD;
    const float elevation = config.sunElevationDegrees * DEG2RAD;

    const float horizontal = std::cos(elevation);

    const Vector3 directionToLight{std::sin(azimuth) * horizontal,std::sin(elevation),std::cos(azimuth) * horizontal};

    const Vector3 sun = MakeLightColor(config.sunColor,config.sunIntensity);

    const Vector3 ambient = MakeLightColor(config.ambientColor,config.ambientIntensity);

    SetShaderValue(_shader,directionLoc,&directionToLight,SHADER_UNIFORM_VEC3);

    SetShaderValue(_shader,sunLoc,&sun,SHADER_UNIFORM_VEC3);

    SetShaderValue(_shader,ambientLoc,&ambient,SHADER_UNIFORM_VEC3);
}

ScaneLight::~ScaneLight() {
    UnloadShader(_shader);
}


Shader ScaneLight::GetShader() const {
    return _shader;
}


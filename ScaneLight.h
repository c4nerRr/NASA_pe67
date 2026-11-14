
#ifndef NASA_SCANELIGHT_H
#define NASA_SCANELIGHT_H
#include "raylib.h"
#include "LightConfig.h"


class ScaneLight {
    Shader _shader{};
public:
    explicit ScaneLight(const LightConfig& config);
    ~ScaneLight();

    ScaneLight(const ScaneLight&) = delete;
    ScaneLight& operator=(const ScaneLight&) = delete;

    [[nodiscard]] Shader GetShader() const;
};


#endif //NASA_SCANELIGHT_H
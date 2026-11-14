
#include "HubOutline.h"

#include "rlgl.h"
#include <stdexcept>

namespace {
     const char* maskFragment = R"glsl(
        #version 330

        uniform float selected;
        out vec4 finalColor;

        void main() {
            finalColor = vec4(vec3(selected), 1.0);
        }
    )glsl";

    const char* outlineFragment = R"glsl(
        #version 330

        in vec2 fragTexCoord;

        uniform sampler2D texture0;
        uniform vec2 texelSize;

        out vec4 finalColor;

        void main() {
            float center = texture(texture0, fragTexCoord).r;

            // Саму базу не закрашиваем.
            if (center > 0.5) discard;

            float edge = 0.0;

            for (int y = -2; y <= 2; ++y) {
                for (int x = -2; x <= 2; ++x) {
                    if (x*x + y*y > 4) continue;

                    vec2 uv = fragTexCoord + vec2(x, y)*texelSize;
                    edge = max(edge, texture(texture0, uv).r);
                }
            }

            if (edge < 0.5) discard;

            finalColor = vec4(1.0);
        }
    )glsl";
}

void HubOutline::DrawMaskObject(const Objects3D &object, bool selected) const {
    const float value = selected ? 1.0f : 0.0f;
    SetShaderValue(_maskShader,_selectedLoc,&value,SHADER_UNIFORM_FLOAT);
    object.DrawWithShader(_maskShader);
}

HubOutline::HubOutline() {
    _maskShader = LoadShaderFromMemory(nullptr, maskFragment);
    _outlineShader = LoadShaderFromMemory(nullptr, outlineFragment);

    _selectedLoc = GetShaderLocation(_maskShader, "selected");
    _texelLoc = GetShaderLocation(_outlineShader, "texelSize");

    if (_selectedLoc < 0 || _texelLoc < 0) {
        UnloadShader(_maskShader);
        UnloadShader(_outlineShader);
        throw std::runtime_error("Failed to load outline shaders");
    }
}

HubOutline::~HubOutline() {
    if (_mask.id != 0) {
        UnloadRenderTexture(_mask);
    }

    UnloadShader(_outlineShader);
    UnloadShader(_maskShader);
}

void HubOutline::Resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    if (_mask.id != 0 &&_mask.texture.width == width &&_mask.texture.height == height) {
        return;
       }

    if (_mask.id != 0) {UnloadRenderTexture(_mask);
        _mask = {};
    }

    _mask = LoadRenderTexture(width, height);

    if (!IsRenderTextureValid(_mask)) {
        throw std::runtime_error("Failed to create outline texture");
    }

    SetTextureFilter(_mask.texture, TEXTURE_FILTER_POINT);
    SetTextureWrap(_mask.texture, TEXTURE_WRAP_CLAMP);

    const Vector2 texelSize{1.0f / width,1.0f / height};

    SetShaderValue(_outlineShader,_texelLoc,&texelSize,SHADER_UNIFORM_VEC2);
}

void HubOutline::Draw(const Camera3D &camera, const Objects3D &moon, const std::vector<HubPortal> &portals,int selected, bool doubleSided) const {
    if (_mask.id == 0 ||
       selected < 0 ||
       selected >= static_cast<int>(portals.size())) {
        return;
       }
    BeginTextureMode(_mask);
    ClearBackground(BLACK);
    BeginMode3D(camera);
    if (doubleSided) {
        rlDisableBackfaceCulling();
    }

    DrawMaskObject(moon, false);

    for (int i = 0; i < static_cast<int>(portals.size()); ++i) {
        DrawMaskObject(portals[i]._visual, i == selected);
    }

    if (doubleSided) {
        rlEnableBackfaceCulling();
    }

    EndMode3D();
    EndTextureMode();

    BeginShaderMode(_outlineShader);

    DrawTextureRec(_mask.texture,{0.0f,0.0f,static_cast<float>(_mask.texture.width),-static_cast<float>(_mask.texture.height)},{0.0f, 0.0f},WHITE);
    EndShaderMode();
}

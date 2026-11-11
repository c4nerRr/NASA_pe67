//
// Created by c4ner on 11/11/2026.
//

#include "GameAssets.h"

#include <ranges>
#include <stdexcept>

#include "rlgl.h"

GameAssets::GameAssets() = default;

GameAssets::~GameAssets() {
    Clear();
}

const Model & GameAssets::Load(const std::string &path, bool emissionAsAlbedo) {
    if (auto it = _models.find(path); it != _models.end()) {
        return it->second;
    }
    if (!FileExists(path.c_str())) {
        throw std::runtime_error("Missing model: " + path);
    }

    Model& model = _models[path];
    model = LoadModel(path.c_str());

    if (!IsModelValid(model)) {
        throw std::runtime_error("Invalid model: " + path);
    }

    //текстури:
    for (int i = 0; i < model.materialCount; ++i) {
        for (int slot = 0; slot <= MATERIAL_MAP_BRDF; ++slot) {
            Texture2D texture =
                model.materials[i].maps[slot].texture;

            if (texture.id && texture.id != rlGetTextureIdDefault()) {
                _textures.emplace(texture.id, texture);
            }
        }
    }
    if (emissionAsAlbedo) {
        for (int i = 0; i < model.meshCount; ++i) {
            auto* maps = model.materials[model.meshMaterial[i]].maps;

            Texture2D texture = maps[MATERIAL_MAP_EMISSION].texture;

            if (!texture.id) {
                throw std::runtime_error( "Missing emission texture: " + path );
            }
            maps[MATERIAL_MAP_ALBEDO].texture = texture;
            maps[MATERIAL_MAP_ALBEDO].color = WHITE;
        }
    }

    return model;
}

void GameAssets::Clear() {
    for (const auto& model : _models | std::views::values) {
        UnloadModel(model);
    }
    _models.clear();

    for (const auto& texture : _textures | std::views::values) {
        UnloadTexture(texture);
    }
    _textures.clear();
}

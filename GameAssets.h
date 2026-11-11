//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_GAMEASSETS_H
#define NASA_GAMEASSETS_H
#include <map>
#include <string>

#include "raylib.h"


class GameAssets {
    std::map<std::string, Model> _models;
    std::map<unsigned int, Texture2D> _textures;

public:
    GameAssets();
    ~GameAssets();

    GameAssets(const GameAssets&) = delete;
    GameAssets& operator=(const GameAssets&)=delete;

    const Model& Load(const std::string& path, bool emissionAsAlbedo);
    void Clear();
};


#endif //NASA_GAMEASSETS_H
//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_HUBPORTAL_H
#define NASA_HUBPORTAL_H
#include "GameManager.h"
#include "raylib.h"


struct HubPortal { //для крутих питань: один куб-вход : тут положення, розмір цвєт і состояніє в котором воно веде
    Vector3 position{};
    Vector3 size{1.2f,1.2f,1.2f};
    Color color = GRAY;

    GameState targetState = GameState::Hub;

    void Draw() const;
    [[nodiscard]] BoundingBox GetBounds()const;
};


#endif //NASA_HUBPORTAL_H
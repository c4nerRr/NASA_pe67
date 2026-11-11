//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_HUBPORTAL_H
#define NASA_HUBPORTAL_H
#include "GameManager.h"
#include "Objects3D.h"
#include "raylib.h"


struct HubPortal {
    Objects3D _visual;
    GameState _targetState;

    HubPortal(const Model& model, Matrix transform, GameState target);
    void Draw() const;

    [[nodiscard]] BoundingBox GetBounds() const;

};


#endif //NASA_HUBPORTAL_H
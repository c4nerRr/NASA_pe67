
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

    void Draw(Shader shader) const;

    [[nodiscard]] BoundingBox GetBounds() const;
    [[nodiscard]] float GetRayHitDistance(Ray ray) const;
};


#endif //NASA_HUBPORTAL_H
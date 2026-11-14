
#include "HubPortal.h"

HubPortal::HubPortal(const Model &model, Matrix transform, GameState target) : _visual(model, transform), _targetState(target) {
}

void HubPortal::Draw() const {
  _visual.Draw();
}

void HubPortal::Draw(Shader shader) const {
    _visual.DrawWithShader(shader);
}

BoundingBox HubPortal::GetBounds() const {
   return _visual.GetBounds();
}

float HubPortal::GetRayHitDistance(Ray ray) const {
    return _visual.GetRayHitDistance(ray);
}

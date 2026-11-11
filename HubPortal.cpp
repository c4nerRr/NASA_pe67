//
// Created by c4ner on 11/11/2026.
//

#include "HubPortal.h"

HubPortal::HubPortal(const Model &model, Matrix transform, GameState target) : _visual(model, transform), _targetState(target) {
}

void HubPortal::Draw() const {
  _visual.Draw();
}

BoundingBox HubPortal::GetBounds() const {
   return _visual.GetBounds();
}

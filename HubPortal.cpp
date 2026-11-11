//
// Created by c4ner on 11/11/2026.
//

#include "HubPortal.h"

void HubPortal::Draw() const {
    DrawCubeV(position, size, color);
    DrawCubeWiresV(position, size, DARKGRAY);
}

BoundingBox HubPortal::GetBounds() const {
    const Vector3 half(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f);

    return { {position.x - half.x, position.y - half.y, position.z - half.z }, {position.x + half.x, position.y + half.y, position.z + half.z}};
}

//
// Created by c4ner on 11/11/2026.
//

#include "Objects3D.h"

#include "raymath.h"

namespace {
    constexpr int rings = 32;
    constexpr int slices = 64;
    constexpr Color moonColor{150, 155, 165, 255};
}


Objects3D::Objects3D() = default;

Objects3D::~Objects3D() = default;

void Objects3D::Draw() const {
    DrawSphereEx(_moonCenter, _moodRadius, rings, slices, moonColor);
}

Vector3 Objects3D::GetSurfacePoint(Vector3 direction) const {
    const Vector3 normal = Vector3Normalize(direction);

    return Vector3Add(_moonCenter, Vector3Scale(normal, _moodRadius));
}

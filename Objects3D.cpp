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


Objects3D::Objects3D(const Model &model, Matrix transform) : _model(model), _transform(transform) {
}

Objects3D::~Objects3D() = default;

void Objects3D::Draw() const {
   Model view = _model;
    view.transform = MatrixMultiply(view.transform, _transform);

    DrawModel(view, {}, 1.0f, WHITE);
}

BoundingBox Objects3D::GetBounds() const {
    const BoundingBox local = GetModelBoundingBox(_model);
    BoundingBox result{};

    for (int i = 0; i < 8; ++i) {
        Vector3 point{
            (i & 1) ? local.max.x : local.min.x,
            (i & 2) ? local.max.y : local.min.y,
            (i & 4) ? local.max.z : local.min.z
        };

        point = Vector3Transform(point, _transform);

        if (i == 0) {
            result = {point, point};
            continue;
        }

        result.min = Vector3Min(result.min, point);
        result.max = Vector3Max(result.max, point);
    }

    return result;
}

float Objects3D::GetRayHitDistance(Ray ray) const {
    const Matrix world = MatrixMultiply(_model.transform, _transform);
    const Matrix inverseWorld = MatrixInvert(world);

    const Vector3 localOrigin = Vector3Transform(ray.position, inverseWorld);

    const Vector3 localEnd = Vector3Transform(Vector3Add(ray.position, ray.direction),inverseWorld);

    const Ray localRay{localOrigin,Vector3Normalize(Vector3Subtract(localEnd, localOrigin))};
    float nearestDistance = std::numeric_limits<float>::infinity();

    for (int i = 0; i < _model.meshCount; ++i) {
        const RayCollision hit = GetRayCollisionMesh(localRay,_model.meshes[i],MatrixIdentity());

        if (!hit.hit) {
            continue;
        }
        const Vector3 worldPoint = Vector3Transform(hit.point, world);
        const float distance = Vector3Distance(ray.position, worldPoint);
        if (distance < nearestDistance) {
            nearestDistance = distance;
        }
    }
    return nearestDistance;
}

void Objects3D::DrawWithShader(Shader shader) const {
    const Matrix world = MatrixMultiply(_model.transform, _transform);

    for (int i = 0; i < _model.meshCount; ++i) {
        Material material = _model.materials[_model.meshMaterial[i]];
        material.shader = shader;

        DrawMesh(_model.meshes[i], material, world);
    }
}

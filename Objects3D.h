
#ifndef NASA_OBJECTS3D_H
#define NASA_OBJECTS3D_H
#include "raylib.h"


class Objects3D {

   const Model& _model;
   Matrix _transform;

public:
   Objects3D(const Model& model, Matrix transform);
   ~Objects3D();

   void Draw() const;
   [[nodiscard]] BoundingBox GetBounds() const;

   [[nodiscard]]float GetRayHitDistance(Ray ray) const;
   void DrawWithShader(Shader shader) const;
};


#endif //NASA_OBJECTS3D_H
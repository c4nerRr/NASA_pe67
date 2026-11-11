//
// Created by c4ner on 11/11/2026.
//

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
};


#endif //NASA_OBJECTS3D_H
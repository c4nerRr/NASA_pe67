//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_OBJECTS3D_H
#define NASA_OBJECTS3D_H
#include "raylib.h"


class Objects3D {
   Vector3 _moonCenter{0.0f, -10.0f, 0.0f};
   float _moodRadius = 10.0f;

public:
   Objects3D();
   ~Objects3D();

   void Draw() const;
   [[nodiscard]] Vector3 GetSurfacePoint(Vector3 direction) const;
};


#endif //NASA_OBJECTS3D_H
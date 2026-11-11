//
// Created by c4ner on 11/11/2026.
//

#ifndef NASA_UI2DALL_H
#define NASA_UI2DALL_H
#include <string>

#include "raylib.h"


class UI2DAll {
    UI2DAll();
    ~UI2DAll();

    void DrawUi(std::string text) const;
    void DrawPhotoMaskot(Texture2D* textur);
};


#endif //NASA_UI2DALL_H
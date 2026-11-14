
#ifndef NASA_INPUTMANAGER_H
#define NASA_INPUTMANAGER_H
#include "raylib.h"


class InputManager {
public:
    static bool PressLeftMouse();
    static bool PressSPACE();
    static bool IsLeftMouseDown();
    static Vector2 MousePosition();
    static bool PressESC();
};


#endif //NASA_INPUTMANAGER_H
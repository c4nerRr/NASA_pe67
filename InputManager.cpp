//
// Created by c4ner on 11/11/2026.
//

#include "InputManager.h"

#include "raylib.h"

bool InputManager::PressLeftMouse() {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

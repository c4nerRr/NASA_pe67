
#include "InputManager.h"

#include "raylib.h"

bool InputManager::PressLeftMouse() {
    return IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

bool InputManager::PressSPACE() {
    return IsKeyPressed(KEY_SPACE);
}

bool InputManager::IsLeftMouseDown() {
    return IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}

Vector2 InputManager::MousePosition() {
    return GetMousePosition();
}

bool InputManager::PressESC() {
    return IsKeyPressed(KEY_ESCAPE);
}

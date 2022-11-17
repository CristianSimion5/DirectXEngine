#include "InputManager.h"

void InputManager::Initialize() {
    for (int i = 0; i < 256; i++) {
        m_Keys[i] = false;
    }
}

void InputManager::KeyDown(unsigned int key) {
    m_Keys[key] = true;
}

void InputManager::KeyUp(unsigned int key) {
    m_Keys[key] = false;
}

bool InputManager::IsKeyDown(unsigned int key) {
    return m_Keys[key];
}

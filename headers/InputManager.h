#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

class InputManager {
public:
    InputManager() = default;
    InputManager(const InputManager&) = default;
    ~InputManager() = default;

    void Initialize();

    void KeyDown(unsigned int);
    void KeyUp(unsigned int);

    bool IsKeyDown(unsigned int);

private:
    bool m_Keys[256];
};

#endif // !_INPUT_MANAGER_H_

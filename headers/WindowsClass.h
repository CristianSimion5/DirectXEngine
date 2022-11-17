#ifndef _WINDOWSCLASS_H_
#define _WINDOWSCLASS_H_

#include <memory>

#include "Helpers.h"

#include "InputManager.h"
#include "GraphicsManager.h"

class WindowsClass {
public:
    WindowsClass() = default;
    WindowsClass(const WindowsClass&) = default;
    ~WindowsClass() = default;

    bool Initialize();
    void Shutdown();
    void Run();

    LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
    bool Frame(float);
    void InitializeWindows(int&, int&);
    void ShutdownWindows();

private:
    LPCWSTR m_ApplicationName;
    HINSTANCE m_hInstance;
    HWND m_hWnd;

    std::unique_ptr<InputManager> m_Input;
    std::unique_ptr<GraphicsManager> m_Graphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static WindowsClass* g_ApplicationHandle;

#endif // !_WINDOWSCLASS_H_

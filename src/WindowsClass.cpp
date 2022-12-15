#include "WindowsClass.h"

#include <iostream>
#include <timeapi.h>

#include <imgui/imgui_impl_win32.h>

// DirectXTK input helper classes
#include "Keyboard.h"
#include "Mouse.h"

using DirectX::Keyboard;
using DirectX::Mouse;

bool WindowsClass::Initialize() {
    int screenWidth;
    int screenHeight;
    bool result;

    InitializeWindows(screenWidth, screenHeight);
    m_Input = std::make_unique<InputManager>();
    m_Input->Initialize();

    m_Graphics = std::make_unique<GraphicsManager>();
    result = m_Graphics->Initialize(screenWidth, screenHeight, m_hWnd);
    if (!result) {
        return false;
    }


    return true;
}

void WindowsClass::Shutdown() {
    if (m_Graphics) {
        m_Graphics->Shutdown();
    }
    ShutdownWindows();

    return;
}

void WindowsClass::Run() {
    MSG msg;
    bool done;
    bool result;
    DWORD previousTime;
    DWORD currentTime;
    float deltaTime;
    const float targetFramerate = 30.0f;
    const float maxTimeStep = 1.0f / targetFramerate;

    ZeroMemory(&msg, sizeof(MSG));

    done = false;
    previousTime = timeGetTime();
    // If Windows signals to end the application then exit
    while (!done && msg.message != WM_QUIT) {
        // Handle the windows messages.
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            currentTime = timeGetTime();
            deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;

            deltaTime = std::min<float>(deltaTime, maxTimeStep);

            // Process a frame
            result = Frame(deltaTime);
            if (!result) {
                done = true;
            }
        }
    }
}

bool WindowsClass::Frame(float deltaTime) {
    bool result;
    

    if (m_Input->IsKeyDown(VK_ESCAPE)) {
        return false;
    }

    result = m_Graphics->Frame(deltaTime);
    if (!result) {
        return false;
    }

    return true;
}

LRESULT CALLBACK WindowsClass::MessageHandler(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_ACTIVATEAPP:
    {
        Keyboard::ProcessMessage(uMsg, wParam, lParam);
        Mouse::ProcessMessage(uMsg, wParam, lParam);
        break;
    }
    case WM_MOUSEACTIVATE:
    {
        return MA_ACTIVATEANDEAT;
    }
    case WM_ACTIVATE :
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
    {
        Mouse::ProcessMessage(uMsg, wParam, lParam);
        break;
    }
    case WM_SYSKEYUP:
    case WM_SYSKEYDOWN:
    {
        Keyboard::ProcessMessage(uMsg, wParam, lParam);
        break;
    }
    case WM_KEYDOWN:
    {
        Keyboard::ProcessMessage(uMsg, wParam, lParam);
        m_Input->KeyDown(static_cast<unsigned int>(wParam));
        return 0;
    }
    case WM_KEYUP:
    {
        Keyboard::ProcessMessage(uMsg, wParam, lParam);
        m_Input->KeyDown(static_cast<unsigned int>(wParam));
        return 0;
    }
    case WM_SIZE:
    {
        if (m_Graphics) {
            // TODO: delete comment after making sure resize works for all cases
            /*auto a = LOWORD(lParam);
            auto b = HIWORD(lParam);*/
            m_Graphics->HandleResize(LOWORD(lParam), HIWORD(lParam));
        }
        return 0;
    }
    default:
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    }
}

void WindowsClass::InitializeWindows(int& screenWidth, int& screenHeight) {
    WNDCLASSEX wndClass;
    DEVMODE dmScreenSettings;
    int posX;
    int posY;

    g_ApplicationHandle = this;

    m_hInstance = GetModuleHandle(nullptr);

    m_ApplicationName = L"DirectXEngine";

    wndClass.style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndClass.lpfnWndProc    = WndProc;
    wndClass.cbClsExtra     = 0;
    wndClass.cbWndExtra     = 0;
    wndClass.hInstance      = m_hInstance;
    wndClass.hIcon          = LoadIcon(nullptr, IDI_WINLOGO);
    wndClass.hIconSm        = wndClass.hIcon;
    wndClass.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground  = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wndClass.lpszMenuName   = nullptr;
    wndClass.lpszClassName  = m_ApplicationName;
    wndClass.cbSize         = sizeof(WNDCLASSEX);

    if (!RegisterClassEx(&wndClass)) {
        std::cerr << "Failed to register the window class.\n";
    }

    screenWidth  = GetSystemMetrics(SM_CXSCREEN);
    screenHeight = GetSystemMetrics(SM_CYSCREEN);

    if (FULL_SCREEN) {
        memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
        dmScreenSettings.dmSize = sizeof(dmScreenSettings);
        dmScreenSettings.dmPelsWidth = screenWidth;
        dmScreenSettings.dmPelsHeight = screenHeight;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

        ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

        posX = posY = 0;
    } else {
        screenWidth = 1280;
        screenHeight = 720;

        posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
        posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
    }

    // Create the window
    m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_ApplicationName, m_ApplicationName,
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, posX, posY, screenWidth, screenHeight,
        nullptr, nullptr, m_hInstance, nullptr);
    if (!m_hWnd) {
        std::cerr << "Failed to create the window.\n";
    }

    ShowWindow(m_hWnd, SW_SHOW);
    SetForegroundWindow(m_hWnd);
    SetFocus(m_hWnd);

    //ShowCursor(FALSE);
}

void WindowsClass::ShutdownWindows() {
    ShowCursor(TRUE);

    if (FULL_SCREEN) {
        ChangeDisplaySettings(nullptr, 0);
    }

    DestroyWindow(m_hWnd);
    m_hWnd = NULL;

    UnregisterClass(m_ApplicationName, m_hInstance);
    m_hInstance = NULL;

    g_ApplicationHandle = nullptr;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMessage, wParam, lParam))
        return true;

    switch (uMessage) {
    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0;
        }
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            //const int dpi = HIWORD(wParam);
            //printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
            const RECT* suggested_rect = (RECT*)lParam;
            SetWindowPos(hWnd, NULL, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        //break;
    default:
        {
            return g_ApplicationHandle->MessageHandler(hWnd, uMessage, wParam, lParam);
        }
    }
}
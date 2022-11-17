#include <memory>

#include "WindowsClass.h"
#include "Helpers.h"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance, 
    _In_opt_ HINSTANCE hPrevInstance, 
    _In_ LPWSTR pScmdline, 
    _In_ int iCmdshow
) {
    std::unique_ptr<WindowsClass> WindowsObj;
    bool result;

    WindowsObj = std::make_unique<WindowsClass>();

    result = WindowsObj->Initialize();
    if (result) {
        WindowsObj->Run();
    }

    WindowsObj->Shutdown();

    return 0;
}
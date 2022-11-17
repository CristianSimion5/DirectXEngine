#ifndef _HELPERS_H_
#define _HELPERS_H_

// Defines
// 'min' macro conflicts with assimp functions, does not work here, also added it in project properties
// #define NOMINMAX

// Exclude lesser used Windows APIs
#define WIN32_LEAN_AND_MEAN

// Includes
#include <Windows.h>

#pragma comment(lib, "winmm.lib")

// Safely release a COM object.
template<typename T>
inline void SafeRelease(T& ptr)
{
    if (ptr != NULL)
    {
        ptr->Release();
        ptr = NULL;
    }
}

#endif // !_HELPERS_H_

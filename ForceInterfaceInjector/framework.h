#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>

// String Header Files
#include <tchar.h>
#include <strsafe.h>
#include <string>

// Library Header Files
#include <debug-helpers.h>
#include <detours.h>

#pragma comment(lib, "detours.lib")

#ifdef _UNICODE
#define String std::wstring
#else
#define String std::string
#endif // !UNICODE

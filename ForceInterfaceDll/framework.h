#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// String Header Files
#include <strsafe.h>

// Library Header Files
#include <debug-helpers.h>
#include <detours.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "detours.lib")

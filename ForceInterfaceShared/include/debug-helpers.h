#pragma once

int __cdecl OutputDebugLineA(LPCSTR format, ...);
int __cdecl OutputDebugLineW(LPCWSTR format, ...);

#ifdef UNICODE
#define OutputDebugLine OutputDebugLineW
#else
#define OutputDebugLine OutputDebugLineA
#endif // !UNICODE

#include "pch.h"
#include <stdio.h>
#include <stdarg.h>

int __cdecl OutputDebugLineA(LPCSTR format, ...)
{
	CHAR szLine[2048];

	va_list argptr;
	va_start(argptr, format);
	int ret = _vsnprintf_s(szLine, sizeof(szLine), format, argptr);
	va_end(argptr);

	OutputDebugStringA(szLine);

	return ret;
}

int __cdecl OutputDebugLineW(LPCWSTR format, ...)
{
	WCHAR szLine[2048];

	va_list argptr;
	va_start(argptr, format);
	int ret = _vsnwprintf_s(szLine, sizeof(szLine), format, argptr);
	va_end(argptr);

	OutputDebugStringW(szLine);

	return ret;
}

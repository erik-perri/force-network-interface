#include "pch.h"
#include "path.h"

LPSTR g_lpDllPath = NULL;

void ClearDllPath()
{
	if (g_lpDllPath != NULL) {
		delete[] g_lpDllPath;
		g_lpDllPath = NULL;
	}
}

void SetDllPath(LPCSTR lpDllPath)
{
	if (g_lpDllPath != NULL) {
		ClearDllPath();
	}

	DWORD dwDllPathSize = (DWORD)strlen(lpDllPath) + 1;
	g_lpDllPath = new CHAR[dwDllPathSize];

	StringCchCopyA(g_lpDllPath, dwDllPathSize, lpDllPath);
}

LPSTR GetDllPath()
{
	return g_lpDllPath;
}

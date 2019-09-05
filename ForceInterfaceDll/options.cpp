#include "pch.h"
#include "options.h"

Options::~Options()
{
	if (m_lpDllPath != NULL) {
		delete[] m_lpDllPath;
		m_lpDllPath = NULL;
	}
}

LPCSTR Options::GetDllPath()
{
	return m_lpDllPath;
}

BOOL Options::LoadDllPath(HMODULE hDllModule)
{
	m_lpDllPath = new CHAR[MAX_PATH];
	DWORD dwRequiredSize = GetModuleFileNameA(hDllModule, m_lpDllPath, MAX_PATH);
	if (dwRequiredSize == 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
		delete[] m_lpDllPath;
		m_lpDllPath = NULL;
		return FALSE;
	}

	return TRUE;
}

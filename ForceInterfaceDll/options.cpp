#include "pch.h"
#include "options.h"

Options::~Options()
{
	if (m_lpDllPath != NULL) {
		delete[] m_lpDllPath;
		m_lpDllPath = NULL;
	}

	if (m_lpInterfaceIp != NULL) {
		delete[] m_lpInterfaceIp;
		m_lpInterfaceIp = NULL;
	}
}

LPCSTR Options::GetDllPath()
{
	return m_lpDllPath;
}

LPCTSTR Options::GetInterfaceIp()
{
	return m_lpInterfaceIp;
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

BOOL Options::LoadInterfaceIpFromEnv(LPCTSTR lpEnvKey)
{
	DWORD dwRequiredSize = GetEnvironmentVariable(lpEnvKey, NULL, 0);
	if (dwRequiredSize == 0) {
		return FALSE;
	}

	m_lpInterfaceIp = new TCHAR[dwRequiredSize];

	if (GetEnvironmentVariable(lpEnvKey, m_lpInterfaceIp, dwRequiredSize) == NULL) {
		delete[] m_lpInterfaceIp;
		m_lpInterfaceIp = NULL;
		return FALSE;
	}

	return TRUE;
}

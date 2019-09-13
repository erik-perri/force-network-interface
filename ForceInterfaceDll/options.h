#pragma once
class Options
{
private:
	LPSTR m_lpDllPath;
	LPTSTR m_lpInterfaceIp;

public:
	~Options();
	LPCSTR GetDllPath();
	LPCTSTR GetInterfaceIp();
	BOOL LoadDllPath(HMODULE hDllModule);
	BOOL LoadInterfaceIpFromEnv(LPCTSTR lpEnvKey);
};

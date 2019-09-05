#pragma once
class Options
{
private:
	LPSTR m_lpDllPath;

public:
	~Options();
	LPCSTR GetDllPath();
	BOOL LoadDllPath(HMODULE hDllModule);
};

Options GetGlobalOptions();

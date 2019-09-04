#include "pch.h"
#include "framework.h"
#include "debug.h"
#include "process.h"

String GetDllPath()
{
	TCHAR szBuffer[MAX_PATH];
	GetModuleFileName(NULL, szBuffer, MAX_PATH);

	String strPath = szBuffer, strDll;

	const size_t stLastSlash = strPath.rfind('\\');
	if (String::npos != stLastSlash) {
		strDll = strPath.substr(0, stLastSlash);
#ifdef _WIN64
		strDll.append(_T("\\ForceInterfaceDll.x64.dll"));
#else
		strDll.append(_T("\\ForceInterfaceDll.x86.dll"));
#endif
	}

	return strDll;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	if (__argc < 2) {
		OutputDebugLine(_T("ForceInterfaceInjector: No app specified"));
		return 3;
	}

	String strCommandLine;
	if (__argc > 2) {
		for (int i = 1; i < __argc; i++) {
			if (strCommandLine.size() > 0) {
				strCommandLine.append(_T(" "));
			}

			strCommandLine.append(_T("\""));
			strCommandLine.append(__targv[i]);
			strCommandLine.append(_T("\""));
		}
	}

	String strDllPath = GetDllPath();

	OutputDebugLine(_T("ForceInterfaceInjector: Creating process %s (%s) with DLL %s"), __targv[1], strCommandLine.c_str(), strDllPath.c_str());

	CreateResult result = CreateProcessWithDll(__targv[1], strCommandLine, strDllPath);
	DWORD dwError = GetLastError();

	switch (result) {
	case SUCCESS:
		OutputDebugLine(_T("ForceInterfaceInjector: - Executed"));
		return 0;
	case INVALID_EXECUTABLE:
		OutputDebugLine(_T("ForceInterfaceInjector: - Invalid executable %s"), __targv[1]);
		return 1;
	case INVALID_DLL:
		OutputDebugLine(_T("ForceInterfaceInjector: - Invalid DLL %s"), strDllPath.c_str());
		return 1;
	case CREATE_FAILED:
		OutputDebugLine(_T("ForceInterfaceInjector: - DetourCreateProcessWithDllEx failed, 0x%08x"), dwError);
		if (dwError == ERROR_INVALID_HANDLE) {
#ifdef _WIN64
			OutputDebugLine(_T("ForceInterfaceInjector: - Can't detour a 32-bit target process from a 64-bit parent process."));
#else
			OutputDebugLine(_T("ForceInterfaceInjector: - Can't detour a 64-bit target process from a 32-bit parent process."));
#endif
		}
		return 1;
	default:
		OutputDebugLine(_T("ForceInterfaceInjector: - Unknown error %d"), (int)result);
		return 1;
	}
}

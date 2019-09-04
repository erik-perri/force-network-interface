#include "pch.h"
#include "framework.h"
#include "process.h"
#include "debug.h"

#include <algorithm>
#include <strsafe.h>
#include <detours.h>

#pragma comment(lib, "detours.lib")

BOOL IsFile(String strPath)
{
	DWORD dwAttrib = GetFileAttributes(strPath.c_str());

	return dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

BOOL EndsWith(String strCheck, String strSuffix)
{
	transform(strCheck.begin(), strCheck.end(), strCheck.begin(), ::tolower);
	transform(strSuffix.begin(), strSuffix.end(), strSuffix.begin(), ::tolower);

	return strCheck.size() >= strSuffix.size()
		&& strCheck.compare(strCheck.size() - strSuffix.size(), strSuffix.size(), strSuffix) == 0;
}

CreateResult CreateProcessWithDll(String strExecutablePath, String strCommandLine, String strDllPath)
{
	if (!IsFile(strExecutablePath) || !EndsWith(strExecutablePath, _T(".exe"))) {
		return INVALID_EXECUTABLE;
	}

	if (!IsFile(strDllPath) || !EndsWith(strDllPath, _T(".dll"))) {
		return INVALID_DLL;
	}

	DWORD nDlls = 1;
	DWORD dwDllLength = (DWORD)strDllPath.size() + 1;
	LPCSTR rpszDllsOut[1];

	for (DWORD n = 0; n < ARRAYSIZE(rpszDllsOut); n++) {
		rpszDllsOut[n] = NULL;
	}

	CHAR* pszDllCopy = new CHAR[dwDllLength];

#ifdef _UNICODE
	const int nSizeNeeded = WideCharToMultiByte(CP_UTF8, 0, &strDllPath[0], (int)strDllPath.size(), NULL, 0, NULL, NULL);

	std::string strConverted(nSizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, &strDllPath[0], (int)strDllPath.size(), &strConverted[0], nSizeNeeded, NULL, NULL);

	StringCchCopyA(pszDllCopy, dwDllLength, strConverted.c_str());
#else
	StringCchCopyA(pszDllCopy, dwDllLength, strDllPath.c_str());
#endif

	rpszDllsOut[0] = pszDllCopy;

	DWORD dwCommandLineLength = (DWORD)strCommandLine.size() + 1;
	TCHAR* pszCommandLine = NULL;
	if (strCommandLine.size() > 0) {
		pszCommandLine = new TCHAR[dwCommandLineLength];
		StringCchCopy(pszCommandLine, dwCommandLineLength, strCommandLine.c_str());
	}

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	SetLastError(0);

	BOOL bCreateResult = DetourCreateProcessWithDlls(strExecutablePath.c_str(), pszCommandLine, NULL, NULL, TRUE, dwFlags, NULL, NULL, &si, &pi, nDlls, rpszDllsOut, NULL);

	for (DWORD n = 0; n < nDlls; n++) {
		if (rpszDllsOut[n] != NULL) {
			delete[] rpszDllsOut[n];
			rpszDllsOut[n] = NULL;
		}
	}

	if (strCommandLine.size() > 0) {
		delete[] pszCommandLine;
		pszCommandLine = NULL;
	}

	if (!bCreateResult) {
		return CREATE_FAILED;
	}

	return SUCCESS;
}

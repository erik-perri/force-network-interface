#include "pch.h"
#include "framework.h"
#include "process.h"

std::wstring GetDllPath()
{
    wchar_t szBuffer[MAX_PATH];

    GetModuleFileNameW(NULL, szBuffer, MAX_PATH);
    PathCchRemoveExtension(szBuffer, MAX_PATH);

#ifdef _WIN64
    return std::wstring(szBuffer).append(L".x64.dll");
#else
    return std::wstring(szBuffer).append(L".x86.dll");
#endif
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    if (__argc < 2) {
        OutputDebugLineW(L"ForceInterfaceInjector: No app specified");
        return 3;
    }

    std::wstring strCommandLine;
    if (__argc > 2) {
        for (int i = 1; i < __argc; i++) {
            if (!strCommandLine.empty()) {
                strCommandLine.append(L" ");
            }

            strCommandLine.append(L"\"");
            strCommandLine.append(__wargv[i]);
            strCommandLine.append(L"\"");
        }
    }

    std::wstring strDllPath = GetDllPath();

    OutputDebugLineW(L"ForceInterfaceInjector: Creating process %s (%s) with DLL %s", __wargv[1], strCommandLine.c_str(), strDllPath.c_str());

    CreateResult result = CreateProcessWithDll(__wargv[1], strCommandLine, strDllPath);
    DWORD dwError = GetLastError();

    switch (result) {
    case SUCCESS:
        OutputDebugLineW(L"ForceInterfaceInjector: - Executed");
        return 0;
    case INVALID_EXECUTABLE:
        OutputDebugLineW(L"ForceInterfaceInjector: - Invalid executable %s", __wargv[1]);
        return 1;
    case INVALID_DLL:
        OutputDebugLineW(L"ForceInterfaceInjector: - Invalid DLL %s", strDllPath.c_str());
        return 1;
    case CREATE_FAILED:
        OutputDebugLineW(L"ForceInterfaceInjector: - DetourCreateProcessWithDllEx failed, 0x%08x", dwError);
        if (dwError == ERROR_INVALID_HANDLE) {
#ifdef _WIN64
            OutputDebugLineW(L"ForceInterfaceInjector: - Can't detour a 32-bit target process from a 64-bit parent process.");
#else
            OutputDebugLineW(L"ForceInterfaceInjector: - Can't detour a 64-bit target process from a 32-bit parent process.");
#endif
        }
        return 1;
    default:
        OutputDebugLineW(L"ForceInterfaceInjector: - Unknown error %d", (int)result);
        return 1;
    }
}

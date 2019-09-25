#include "pch.h"
#include "framework.h"
#include "injector-options.h"

DWORD CreateProcessWithDll(InjectorOptions options);

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nShowCmd
)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nShowCmd);

    InjectorOptions options;

    options.LoadFromEnvironment(hInstance);
    options.LoadFromCommandLine(__argc, __wargv);

    if (options.GetDllPath() == nullptr)
    {
        OutputDebugLineW(L"Failed to find DLL path to inject.");
        return 1;
    }

    if (options.GetInterfaceIp() == nullptr)
    {
        OutputDebugLineW(L"No interface IP specified.");
        return 1;
    }

    if (options.GetProcessCommandLine() == nullptr)
    {
        OutputDebugLineW(L"No launch executable specified.");
        return 1;
    }

    OutputDebugLineW(L"Creating process");
    OutputDebugLineW(L" - Process: %s", options.GetProcessCommandLine());
    OutputDebugLineW(L" - DLL:     %S", options.GetDllPath());

    DWORD dwCreatedId = CreateProcessWithDll(options);
    if (dwCreatedId != 0) {
        OutputDebugLineW(L" - Created: %d", dwCreatedId);
        return 0;
    }

    DWORD dwLastError = GetLastError();

    OutputDebugLineW(L" - Failed to create, last error: 0x%08x (%d)", dwLastError, dwLastError);
    if (dwLastError == ERROR_INVALID_HANDLE) {
#ifdef _WIN64
        OutputDebugLineW(L" - Can't detour a 32-bit target process from a 64-bit parent process.");
#else
        OutputDebugLineW(L" - Can't detour a 64-bit target process from a 32-bit parent process.");
#endif
    }

    return 1;
}

DWORD CreateProcessWithDll(InjectorOptions options)
{
    // CreateProcessW can modify the command line so it can't be const
    size_t nCommandLineSize = wcslen(options.GetProcessCommandLine()) + 1;
    wchar_t* pszCommandLine = new wchar_t[nCommandLineSize];
    StringCchCopyW(pszCommandLine, nCommandLineSize, options.GetProcessCommandLine());

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD dwFlags = CREATE_DEFAULT_ERROR_MODE;

    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    SetLastError(0);

    SetEnvironmentVariableW(options.GetInterfaceIpEnvKey(), options.GetInterfaceIp());

    BOOL bCreateResult = DetourCreateProcessWithDllExW(
        nullptr, pszCommandLine, NULL, NULL, TRUE, dwFlags,
        NULL, NULL, &si, &pi, options.GetDllPath(), NULL
    );

    delete[] pszCommandLine;

    if (bCreateResult) {
        return pi.dwProcessId;
    }

    return 0;
}

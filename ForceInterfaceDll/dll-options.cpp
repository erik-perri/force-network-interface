#include "pch.h"
#include "dll-options.h"

void DllOptions::LoadFromEnvironment(HMODULE hModule)
{
    this->strInterfaceIp = LoadEnv(GetInterfaceIpEnvKey());
    this->strDllPath = LoadDllPath(hModule);
}

std::string DllOptions::LoadDllPath(HMODULE hDllModule)
{
    char szBuffer[MAX_PATH];

    DWORD dwRequiredSize = GetModuleFileNameA(hDllModule, szBuffer, MAX_PATH);
    if (dwRequiredSize == 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
        return {};
    }

    return std::string(szBuffer);
}

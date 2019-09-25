#include "pch.h"
#include "common-options.h"

LPCWSTR CommonOptions::GetInterfaceIpEnvKey()
{
    return L"FORCE_INTERFACE_IP";
}

LPCWSTR CommonOptions::GetInterfaceIp()
{
    return this->strInterfaceIp.empty()
        ? nullptr
        : this->strInterfaceIp.c_str();
}

LPCSTR CommonOptions::GetDllPath()
{
    return this->strDllPath.empty()
        ? nullptr
        : this->strDllPath.c_str();
}

std::wstring CommonOptions::LoadEnv(LPCWSTR lpEnvKey)
{
    DWORD nRequiredChars = GetEnvironmentVariableW(lpEnvKey, NULL, 0);
    if (nRequiredChars == 0) {
        return {};
    }

    std::unique_ptr<wchar_t[]> pBuffer(new wchar_t[nRequiredChars]);

    DWORD nAddedChars = GetEnvironmentVariableW(lpEnvKey, pBuffer.get(), nRequiredChars);
    if (FAILED(nAddedChars)) {
        return {};
    }

    return std::wstring(pBuffer.get(), pBuffer.get() + nAddedChars);
}

#pragma once

#include <string>

class CommonOptions
{
public:
    LPCWSTR GetInterfaceIpEnvKey();

    LPCWSTR GetInterfaceIp();
    LPCSTR GetDllPath();

protected:
    std::wstring LoadEnv(LPCWSTR lpEnvKey);

    // The DLL name is always used as LPCSTR so there is no point storing it as wchar_t
    std::string strDllPath;
    std::wstring strInterfaceIp;
};

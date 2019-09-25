#pragma once

#include <common-options.h>

class InjectorOptions :
    public CommonOptions
{
public:
    LPCWSTR GetProcessCommandLine();

    void LoadFromCommandLine(int argc, wchar_t* argv[]);
    void LoadFromEnvironment(HMODULE hModule);

private:
    std::string LoadDllPath(HMODULE hInjectorModule);

    std::wstring strCommandLine;
};

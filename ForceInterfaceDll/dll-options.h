#pragma once

#include <common-options.h>

class DllOptions :
    public CommonOptions
{
public:
    void LoadFromEnvironment(HMODULE hModule);

private:
    std::string LoadDllPath(HMODULE hDllModule);
};

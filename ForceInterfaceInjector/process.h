#pragma once

enum CreateResult
{
    SUCCESS = 0,
    INVALID_EXECUTABLE,
    INVALID_DLL,
    CREATE_FAILED
};

CreateResult CreateProcessWithDll(std::wstring strExecutablePath, std::wstring strCommandLine, std::wstring strDllPath);

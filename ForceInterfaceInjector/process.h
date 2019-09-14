#pragma once

enum CreateResult
{
    SUCCESS = 0,
    INVALID_EXECUTABLE,
    INVALID_DLL,
    CREATE_FAILED
};

CreateResult CreateProcessWithDll(String strExecutablePath, String strCommandLine, String strDllPath);

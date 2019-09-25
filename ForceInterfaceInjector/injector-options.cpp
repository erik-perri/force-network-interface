#include "pch.h"
#include "injector-options.h"

LPCWSTR InjectorOptions::GetProcessCommandLine()
{
    return this->strCommandLine.empty()
        ? nullptr
        : this->strCommandLine.c_str();
}

void InjectorOptions::LoadFromCommandLine(int argc, wchar_t* argv[])
{
    this->strCommandLine = L"";

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!this->strCommandLine.empty()) {
                this->strCommandLine.append(L" ");
            }

            this->strCommandLine.append(L"\"");
            this->strCommandLine.append(__wargv[i]);
            this->strCommandLine.append(L"\"");
        }
    }

    // TODO Add loading of interface IP from command line
}

void InjectorOptions::LoadFromEnvironment(HMODULE hModule)
{
    this->strInterfaceIp = LoadEnv(GetInterfaceIpEnvKey());
    this->strDllPath = LoadDllPath(hModule);
}

std::string InjectorOptions::LoadDllPath(HMODULE hInjectorModule)
{
    wchar_t szBuffer[MAX_PATH];

    GetModuleFileNameW(hInjectorModule, szBuffer, MAX_PATH);
    PathCchRemoveExtension(szBuffer, MAX_PATH);

#ifdef _WIN64
    std::wstring strDllPath = std::wstring(szBuffer).append(L".x64.dll");
#else
    std::wstring strDllPath = std::wstring(szBuffer).append(L".x86.dll");
#endif

    const int nSizeNeeded = WideCharToMultiByte(CP_UTF8, 0, strDllPath.c_str(), (int)strDllPath.size(), NULL, 0, NULL, NULL);

    std::unique_ptr<char[]> pBuffer(new char[nSizeNeeded]);

    const int nSizeAdded = WideCharToMultiByte(CP_UTF8, 0, strDllPath.c_str(), (int)strDllPath.size(), pBuffer.get(), nSizeNeeded, NULL, NULL);

    return std::string(pBuffer.get(), pBuffer.get() + nSizeAdded);
}

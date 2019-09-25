#include "pch.h"
#include "detour.h"
#include "dll-options.h"

// Detours requires an export in the DLL to inject properly
void __declspec(dllexport) Unused()
{
}

DllOptions options;
bool attached = false;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);

    switch (dwReasonForCall) {
    case DLL_PROCESS_ATTACH:
        OutputDebugLineW(L"Attaching DLL to %p (%d)", hModule, GetCurrentProcessId());

        options.LoadFromEnvironment(hModule);

        if (options.GetDllPath() != nullptr && options.GetInterfaceIp() != nullptr) {
            attached = AttachDetours(options.GetDllPath(), options.GetInterfaceIp());
            if (!attached) {
                OutputDebugLineW(L" - Failed to attach detours");
            }
        }
        else {
            attached = false;
            OutputDebugLineW(L" - Unable to attach, missing options");
        }

        break;
    case DLL_PROCESS_DETACH:
        OutputDebugLineW(L"Detaching DLL from %p (%d)", hModule, GetCurrentProcessId());

        if (attached) {
            DetachDetours();
        }

        break;
    }
    return TRUE;
}

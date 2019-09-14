#include "pch.h"
#include "options.h"
#include "detour.h"

// Detours requires an export in the DLL to inject properly
void __declspec(dllexport) Unused()
{
}

Options g_forceOptions;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved)
{
    switch (dwReasonForCall) {
    case DLL_PROCESS_ATTACH:
        if (!g_forceOptions.LoadInterfaceIpFromEnv(_T("FORCE_INTERFACE"))) {
            OutputDebugLine(_T("Not attaching to %d"), GetCurrentProcessId());
            return TRUE;
        }

        g_forceOptions.LoadDllPath(hModule);

        OutputDebugLine(_T("Attaching to %d, binding to %s"), GetCurrentProcessId(), g_forceOptions.GetInterfaceIp());

        AttachDetours(g_forceOptions.GetDllPath(), g_forceOptions.GetInterfaceIp());
        break;
    case DLL_PROCESS_DETACH:
        OutputDebugLine(_T("Detaching from %d"), GetCurrentProcessId());

        DetachDetours();
        break;
    }
    return TRUE;
}

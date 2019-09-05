#include "pch.h"
#include "options.h"
#include "detour.h"

// Detours requires an export in the DLL to inject properly
void __declspec(dllexport) Unused()
{
}

Options g_forceOptions;

Options GetGlobalOptions()
{
	return g_forceOptions;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved)
{
	CHAR szDllPath[MAX_PATH];
	HMODULE hDllModule = NULL;

	switch (dwReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		g_forceOptions.LoadDllPath(hModule);

		OutputDebugLine(_T("ForceInterfaceDll: ProcessAttach() %d %s"), GetCurrentProcessId(), g_forceOptions.GetDllPath());

		AttachDetours(g_forceOptions.GetDllPath());
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugLine(_T("ForceInterfaceDll: ProcessDetach() %d"), GetCurrentProcessId());

		DetachDetours();
		break;
	}
	return TRUE;
}

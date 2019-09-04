#include "pch.h"
#include "detour.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringW(L"ForceInterfaceDll: DLL_PROCESS_ATTACH");

		AttachDetours();
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugStringW(L"ForceInterfaceDll: DLL_PROCESS_DETACH");

		DetachDetours();
		break;
	}
	return TRUE;
}

// Detours requires an export in the DLL to inject properly
void __declspec(dllexport) Unused()
{
}

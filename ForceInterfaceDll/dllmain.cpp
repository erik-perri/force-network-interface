#include "pch.h"
#include "detour.h"
#include "path.h"

// Detours requires an export in the DLL to inject properly
void __declspec(dllexport) Unused()
{
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved)
{
	CHAR szDllPath[MAX_PATH];
	HMODULE hDllModule = NULL;

	switch (dwReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugStringW(L"ForceInterfaceDll: DLL_PROCESS_ATTACH");

		ClearDllPath();

		if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)& Unused, &hDllModule) != 0
			&& GetModuleFileNameA(hDllModule, szDllPath, sizeof(szDllPath)) != 0)
		{
			SetDllPath(szDllPath);
		}

		AttachDetours();
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugStringW(L"ForceInterfaceDll: DLL_PROCESS_DETACH");

		DetachDetours();
		ClearDllPath();
		break;
	}
	return TRUE;
}

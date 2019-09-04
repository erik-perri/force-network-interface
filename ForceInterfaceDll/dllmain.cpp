#include "pch.h"
#include <WinSock2.h>
#include <detours.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "detours.lib")

int (WINAPI* Real_send)(SOCKET s, CONST char* buf, int len, int flags) = send;

int WINAPI Hooked_send(SOCKET s, char* buf, int len, int flags)
{
	OutputDebugString(L"ForceNetworkInterface: ForceInterfaceDll: Hooked_send");

	return Real_send(s, buf, len, flags);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		OutputDebugString(L"ForceNetworkInterface: ForceInterfaceDll: DLL_PROCESS_ATTACH");

		DetourRestoreAfterWith();

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourAttach(&(PVOID&)Real_send, Hooked_send);

		DetourTransactionCommit();
		break;
	case DLL_PROCESS_DETACH:
		OutputDebugString(L"ForceNetworkInterface: ForceInterfaceDll: DLL_PROCESS_DETACH");

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourDetach(&(PVOID&)Real_send, Hooked_send);

		DetourTransactionCommit();
		break;
	}
	return TRUE;
}

// Detours requires an export in the DLL to inject properly
void __declspec(dllexport) Unused()
{
}

#include "pch.h"

#include <WinSock2.h>
#include <detours.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "detours.lib")

int (WINAPI* Real_send)(SOCKET s, CONST char* buf, int len, int flags) = send;

int WINAPI Hooked_send(SOCKET s, char* buf, int len, int flags)
{
	OutputDebugStringW(L"ForceNetworkInterface: ForceInterfaceDll: Hooked_send");

	return Real_send(s, buf, len, flags);
}

void AttachDetours()
{
	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)Real_send, Hooked_send);

	DetourTransactionCommit();
}

void DetachDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(PVOID&)Real_send, Hooked_send);

	DetourTransactionCommit();
}

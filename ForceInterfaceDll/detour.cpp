#include "pch.h"
#include "path.h"

#include <WinSock2.h>
#include <detours.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "detours.lib")

int (WINAPI* Real_send)(SOCKET s, CONST char* buf, int len, int flags) = send;

int WINAPI Hooked_send(SOCKET s, char* buf, int len, int flags)
{
	OutputDebugStringW(L"ForceInterfaceDll: Hooked_send");

	return Real_send(s, buf, len, flags);
}

/*
 * Hook CreateProcessA to attach the DLL to any sub-processes
 */

BOOL(WINAPI* Real_CreateProcessA)(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation) = CreateProcessA;

BOOL WINAPI Hooked_CreateProcessA(LPCSTR lpApplicationName, LPSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCSTR lpCurrentDirectory, LPSTARTUPINFOA lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	LPCSTR lpDllPath = GetDllPath();
	if (lpDllPath != NULL)
	{
		return DetourCreateProcessWithDllExA(
			lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation,
			lpDllPath, Real_CreateProcessA
		);
	}

	return Real_CreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

/*
 * Hook CreateProcessW to attach the DLL to any sub-processes
 */

BOOL(WINAPI* Real_CreateProcessW)(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation) = CreateProcessW;

BOOL WINAPI Hooked_CreateProcessW(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	LPCSTR lpDllPath = GetDllPath();
	if (lpDllPath != NULL)
	{
		return DetourCreateProcessWithDllExW(
			lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation,
			lpDllPath, Real_CreateProcessW
		);
	}

	return Real_CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
}

void AttachDetours()
{
	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)Real_send, Hooked_send);
	DetourAttach(&(PVOID&)Real_CreateProcessA, Hooked_CreateProcessA);
	DetourAttach(&(PVOID&)Real_CreateProcessW, Hooked_CreateProcessW);

	DetourTransactionCommit();
}

void DetachDetours()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(PVOID&)Real_send, Hooked_send);
	DetourDetach(&(PVOID&)Real_CreateProcessA, Hooked_CreateProcessA);
	DetourDetach(&(PVOID&)Real_CreateProcessW, Hooked_CreateProcessW);

	DetourTransactionCommit();
}

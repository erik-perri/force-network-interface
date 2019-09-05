#include "pch.h"

BOOL g_bAttached = FALSE;
LPCSTR g_pDllPath = NULL;
in_addr g_addrSocketAddr;

/*
 * Hook bind to override the address
 */

int (WINAPI* Real_bind)(
	SOCKET s,
	const struct sockaddr* name,
	int namelen
	) = bind;

int WINAPI Hooked_bind(
	SOCKET s,
	const struct sockaddr* name,
	int namelen
)
{
	if (name->sa_family != AF_INET || g_addrSocketAddr.S_un.S_addr == 0) {
		return Real_bind(s, name, namelen);
	}

	sockaddr_in addrCopy = {};
	memcpy(&addrCopy, name, sizeof(struct sockaddr_in));

	addrCopy.sin_addr = g_addrSocketAddr;

	return Real_bind(s, (struct sockaddr*) & addrCopy, namelen);
}

/*
 * Hook connect to bind the socket if it is not bound yet
 */

int (WINAPI* Real_connect)(
	SOCKET s,
	const struct sockaddr* name,
	int namelen
	) = connect;

int WINAPI Hooked_connect(
	SOCKET s,
	const struct sockaddr* name,
	int namelen
)
{
	if (name->sa_family != AF_INET || g_addrSocketAddr.S_un.S_addr == 0) {
		return Real_connect(s, name, namelen);
	}

	sockaddr_in addrInfo = {};
	int nInfoLength = sizeof(struct sockaddr_in);

	if (getsockname(s, ((struct sockaddr*) & addrInfo), &nInfoLength) == SOCKET_ERROR && WSAGetLastError() != WSAEINVAL) {
		OutputDebugLine(
			_T("ForceInterfaceDll: Hooked_connect(%p, %p, %d): getsockname failed %d"),
			s, name, namelen, WSAGetLastError()
		);
		return Real_connect(s, name, namelen);
	}

	if (addrInfo.sin_family == AF_UNSPEC) {
		addrInfo = {};
		addrInfo.sin_family = AF_INET;
		addrInfo.sin_addr = g_addrSocketAddr;
		addrInfo.sin_port = 0;

		if (Real_bind(s, (struct sockaddr*) & addrInfo, nInfoLength) == SOCKET_ERROR) {
			OutputDebugLine(
				_T("ForceInterfaceDll: Hooked_connect(%p, %p, %d): bind failed %d"),
				s, name, namelen, WSAGetLastError()
			);
			return SOCKET_ERROR;
		}

		OutputDebugLine(_T("ForceInterfaceDll: Hooked_connect(%p, %p, %d): bind success"), s, name, namelen);
	}

	return Real_connect(s, name, namelen);
}

/*
 * Hook CreateProcessA to attach the DLL to any sub-processes
 */

BOOL(WINAPI* Real_CreateProcessA)(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	) = CreateProcessA;

BOOL WINAPI Hooked_CreateProcessA(
	LPCSTR lpApplicationName,
	LPSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCSTR lpCurrentDirectory,
	LPSTARTUPINFOA lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	if (g_pDllPath != NULL) {
		return DetourCreateProcessWithDllExA(
			lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
			dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation,
			g_pDllPath, Real_CreateProcessA
		);
	}

	return Real_CreateProcessA(
		lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
		dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation
	);
}

/*
 * Hook CreateProcessW to attach the DLL to any sub-processes
 */

BOOL(WINAPI* Real_CreateProcessW)(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	) = CreateProcessW;

BOOL WINAPI Hooked_CreateProcessW(
	LPCWSTR lpApplicationName,
	LPWSTR lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL bInheritHandles,
	DWORD dwCreationFlags,
	LPVOID lpEnvironment,
	LPCWSTR lpCurrentDirectory,
	LPSTARTUPINFOW lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
)
{
	if (g_pDllPath != NULL) {
		return DetourCreateProcessWithDllExW(
			lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
			dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation,
			g_pDllPath, Real_CreateProcessW
		);
	}

	return Real_CreateProcessW(
		lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
		dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation
	);
}

void AttachDetours(LPCSTR lpDllPath, LPCTSTR lpIpAddress)
{
	g_pDllPath = lpDllPath;
	g_addrSocketAddr = {};

	if (InetPton(AF_INET, lpIpAddress, &g_addrSocketAddr) < 1) {
		g_addrSocketAddr = {};
		return;
	}

	DetourRestoreAfterWith();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourAttach(&(PVOID&)Real_bind, Hooked_bind);
	DetourAttach(&(PVOID&)Real_connect, Hooked_connect);
	DetourAttach(&(PVOID&)Real_CreateProcessA, Hooked_CreateProcessA);
	DetourAttach(&(PVOID&)Real_CreateProcessW, Hooked_CreateProcessW);

	DetourTransactionCommit();

	g_bAttached = TRUE;
}

void DetachDetours()
{
	if (g_bAttached == FALSE) {
		return;
	}

	g_pDllPath = NULL;
	g_addrSocketAddr = {};

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

	DetourDetach(&(PVOID&)Real_bind, Hooked_bind);
	DetourDetach(&(PVOID&)Real_connect, Hooked_connect);
	DetourDetach(&(PVOID&)Real_CreateProcessA, Hooked_CreateProcessA);
	DetourDetach(&(PVOID&)Real_CreateProcessW, Hooked_CreateProcessW);

	DetourTransactionCommit();
}

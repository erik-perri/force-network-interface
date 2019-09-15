#include "pch.h"
#include "debug-helpers.h"

int __cdecl OutputDebugLineA(LPCSTR lpszFormat, ...)
{
    va_list argList;
    va_start(argList, lpszFormat);

    size_t nRequiredChars = _vscprintf(lpszFormat, argList);
    if (nRequiredChars == -1) {
        va_end(argList);
        return -1;
    }

    nRequiredChars += (size_t)1;  // Add 1 for the terminating null character

    std::unique_ptr<char[]> pBuffer(new char[nRequiredChars]);
    int nCharsAdded = _vsnprintf_s(pBuffer.get(), nRequiredChars, _TRUNCATE, lpszFormat, argList);

    va_end(argList);

    if (nCharsAdded != -1) {
        std::string strLine(pBuffer.get(), pBuffer.get() + nCharsAdded);
        strLine.append("\r\n");

        OutputDebugStringA(strLine.c_str());
    }

    return nCharsAdded;
}

int __cdecl OutputDebugLineW(LPCWSTR lpszFormat, ...)
{
    va_list argList;
    va_start(argList, lpszFormat);

    size_t nRequiredChars = _vscwprintf(lpszFormat, argList);
    if (nRequiredChars == -1) {
        va_end(argList);
        return -1;
    }

    nRequiredChars += (size_t)1;  // Add 1 for the terminating null character

    std::unique_ptr<wchar_t[]> pBuffer(new wchar_t[nRequiredChars]);
    int nCharsAdded = _vsnwprintf_s(pBuffer.get(), nRequiredChars, _TRUNCATE, lpszFormat, argList);

    va_end(argList);

    if (nCharsAdded != -1) {
        std::wstring strLine(pBuffer.get(), pBuffer.get() + nCharsAdded);
        strLine.append(L"\r\n");

        OutputDebugStringW(strLine.c_str());
    }

    return nCharsAdded;
}

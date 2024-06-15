#pragma once

#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <string>
#include <windows.h>
#include <tchar.h>
#include <cstdint>

#include <Dbghelp.h>

#pragma comment(lib, "Dbghelp")


namespace CppMiniToolkit
{
    namespace Windows
    {
        class ErrorHandling
        {
            ErrorHandling() = delete;
            ~ErrorHandling() = delete;
        public:
            typedef std::basic_string<TCHAR> StringT;

            static StringT FormatErrorMessage(DWORD errorCode)
            {
                LPVOID lpMsgBuf = nullptr;
                DWORD bufLen = FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&lpMsgBuf,
                    0, NULL);

                if (bufLen)
                {                    
                    LPCTSTR lpMsgStr = (LPCTSTR)lpMsgBuf;

                    StringT Result(lpMsgStr, lpMsgStr + bufLen);

                    LocalFree(lpMsgBuf);

                    return Result;
                }

                return StringT();
            }

            static StringT FormatBytes(uint64_t bytes)
            {
                static const TCHAR* units[] = { _T("Bytes"), _T("KB"), _T("MB"), _T("GB"), _T("TB") };

                double finalVal = (double)bytes;
                int i = 0;

                for (i = 0; finalVal >= 1024 && i < 4; i++)
                {
                    finalVal /= 1024;
                }

                TCHAR buffer[64] = { 0 };

                _stprintf_s(buffer, _T("%.2f %s"), finalVal, units[i]);

                return buffer;
            }

            static bool DumpProcess(int processId, const StringT& path)
            {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                if (hProcess == NULL)
                {                    
                    return false;
                }

                CPPMINITOOLKIT_SCOPED_EXIT(CloseHandle(hProcess));

                HANDLE hFile = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

                if (hFile == INVALID_HANDLE_VALUE)
                {
                    return false;
                }

                CPPMINITOOLKIT_SCOPED_EXIT(CloseHandle(hFile));

                // Ð´Èë minidump
                MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
                exceptionInfo.ThreadId = GetCurrentThreadId();
                exceptionInfo.ExceptionPointers = NULL;
                exceptionInfo.ClientPointers = FALSE;
                BOOL result = MiniDumpWriteDump(hProcess, processId, hFile, MiniDumpNormal, &exceptionInfo, NULL, NULL);
                if (!result)
                {
                    // Failed ? 
                    return false;
                }

                return true;
            }
        };
    }
}
#endif

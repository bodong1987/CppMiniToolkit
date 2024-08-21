#pragma once

#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <string>
#include <windows.h>
#include <cstdint>

#include <Dbghelp.h>

#pragma comment(lib, "Dbghelp")


namespace CppMiniToolkit
{
    namespace Windows
    {
        class ErrorHandling
        {
        public:
            ErrorHandling() = delete;
            ~ErrorHandling() = delete;

            typedef std::basic_string<TCHAR> StringT;

            static StringT FormatErrorMessage(DWORD errorCode)
            {
                LPVOID lpMsgBuf = nullptr;
                const DWORD bufLen = FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr,
                    errorCode,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    reinterpret_cast<LPTSTR>(&lpMsgBuf),
                    0, nullptr);

                if (bufLen)
                {
                    const auto lpMsgStr = static_cast<LPCTSTR>(lpMsgBuf);

                    StringT Result(lpMsgStr, lpMsgStr + bufLen);

                    LocalFree(lpMsgBuf);

                    return Result;
                }

                return {};
            }

            static StringT FormatBytes(uint64_t bytes)
            {
                static const TCHAR* units[] = { _T("Bytes"), _T("KB"), _T("MB"), _T("GB"), _T("TB") };

                double finalVal = static_cast<double>(bytes);

                int Index = 0;
                for (; finalVal >= 1024 && Index < 4; Index++)
                {
                    finalVal /= 1024;
                }

                TCHAR buffer[64] = { 0 };

                _stprintf_s(buffer, _T("%.2f %s"), finalVal, units[Index]);

                return buffer;
            }

            static bool DumpProcess(int processId, const StringT& path)
            {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
                if (hProcess == nullptr)
                {                    
                    return false;
                }

                CPPMINITOOLKIT_SCOPED_EXIT(CloseHandle(hProcess));

                HANDLE hFile = CreateFile(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

                if (hFile == INVALID_HANDLE_VALUE)
                {
                    return false;
                }

                CPPMINITOOLKIT_SCOPED_EXIT(CloseHandle(hFile));

                // д�� minidump
                MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
                exceptionInfo.ThreadId = GetCurrentThreadId();
                exceptionInfo.ExceptionPointers = nullptr;
                exceptionInfo.ClientPointers = FALSE;
                BOOL result = MiniDumpWriteDump(hProcess, processId, hFile, MiniDumpNormal, &exceptionInfo, nullptr, nullptr);
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

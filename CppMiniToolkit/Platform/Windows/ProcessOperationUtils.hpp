#pragma once

#include <Common/BuildConfig.hpp>
#include <Common/ScopedExit.hpp>

#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
#include <windows.h>
#include <cassert>
#include <string>
#include <winternl.h>

namespace CppMiniToolkit
{
    namespace Windows
    {
        class ProcessOperationUtils
        {
        public:
            CPP_MINI_TOOLKIT_DECLARE_TOOLKIT_CLASS_TYPE(ProcessOperationUtils);

            // ReSharper disable CppInconsistentNaming
            typedef struct {
                PVOID Reserved1;
                PVOID PebBaseAddress;
                PVOID Reserved2[2];
                ULONG_PTR UniqueProcessId;
                ULONG_PTR InheritedFromUniqueProcessId;
            } PROCESS_BASIC_INFORMATION;
            
            typedef NTSTATUS(WINAPI* NtQueryInformationProcessFunctionType)(                
                HANDLE ProcessHandle,
                ULONG ProcessInformationClass,
                PVOID ProcessInformation,
                ULONG ProcessInformationLength,
                PULONG ReturnLength);
            // ReSharper restore CppInconsistentNaming

            static NtQueryInformationProcessFunctionType GetNtDllNtQueryInformationProcessFunction()
            {
                static NtQueryInformationProcessFunctionType NtQueryInformationProcess = nullptr;

                if(NtQueryInformationProcess == nullptr)
                {
                    const auto moduleHandle = GetModuleHandleW(L"ntdll.dll");

                    if(moduleHandle == nullptr)
                    {
                        return nullptr;
                    }

                    NtQueryInformationProcess = reinterpret_cast<NtQueryInformationProcessFunctionType>(GetProcAddress(moduleHandle, "NtQueryInformationProcess"));  // NOLINT(clang-diagnostic-cast-function-type-strict)

                    assert(NtQueryInformationProcess != nullptr);
                }

                return NtQueryInformationProcess;
            }

            static DWORD GetParentProcessId(HANDLE hProcess)
            {
                static NtQueryInformationProcessFunctionType NtQueryInformationProcess = GetNtDllNtQueryInformationProcessFunction();
                assert(NtQueryInformationProcess != nullptr);

                PROCESS_BASIC_INFORMATION pbi;
                ULONG returnLength;
                
                const NTSTATUS status = NtQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), &returnLength);
                if (status != 0)
                {
                    return 0;
                }

                return static_cast<DWORD>(pbi.InheritedFromUniqueProcessId);
            }

            static DWORD GetParentProcessId(const DWORD processId)
            {
                const auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);

                if(hProcess == nullptr)
                {
                    return 0;
                }

                CPP_MINI_TOOLKIT_SCOPED_EXIT(CloseHandle(hProcess));

                return GetParentProcessId(hProcess);
            }

            static std::wstring GetProcessPath(const DWORD processId)
            {
                const auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, processId);

                if(hProcess == nullptr)
                {
                    return {};
                }

                CPP_MINI_TOOLKIT_SCOPED_EXIT(CloseHandle(hProcess));

                return GetProcessPath(hProcess);
            }

            static std::wstring GetProcessPath(HANDLE hProcess)
            {
                static NtQueryInformationProcessFunctionType NtQueryInformationProcess = GetNtDllNtQueryInformationProcessFunction();
                assert(NtQueryInformationProcess != nullptr);
                
                PROCESS_BASIC_INFORMATION pbi;
                ULONG returnLength;
                const NTSTATUS status = NtQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), &returnLength);
                if (status != 0)
                {
                    return {};
                }

                const PVOID pebAddress = pbi.PebBaseAddress;
                PVOID rtlUserProcParamsAddress;
                if (!ReadProcessMemory(hProcess, static_cast<PCHAR>(pebAddress) + 0x20, &rtlUserProcParamsAddress, sizeof(PVOID), nullptr))  // NOLINT(bugprone-multi-level-implicit-pointer-conversion)
                {
                    return {};
                }

                UNICODE_STRING commandLine;
                if (!ReadProcessMemory(hProcess, static_cast<PCHAR>(rtlUserProcParamsAddress) + 0x70, &commandLine, sizeof(commandLine), nullptr))
                {
                    return {};
                }

                const auto commandLineContents = new WCHAR[commandLine.Length / 2 + 1];
                CPP_MINI_TOOLKIT_SCOPED_EXIT(delete[] commandLineContents);
                if (!ReadProcessMemory(hProcess, commandLine.Buffer, commandLineContents, commandLine.Length, nullptr))
                {   
                    return {};
                }

                commandLineContents[commandLine.Length / 2] = 0;

                std::wstring commandLineWStr(commandLineContents);
                return commandLineWStr;
            }
        };
    }
}
#endif

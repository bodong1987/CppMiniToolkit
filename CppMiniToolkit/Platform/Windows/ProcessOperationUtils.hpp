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
    namespace PlatformWindows
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

            // ReSharper disable once CppParameterMayBeConst
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

                // ReSharper disable once CppRedundantCastExpression
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

            // ReSharper disable once CppParameterMayBeConst
            static std::wstring GetProcessPath(HANDLE hProcess)
            {
                static NtQueryInformationProcessFunctionType NtQueryInformationProcess = GetNtDllNtQueryInformationProcessFunction();
                assert(NtQueryInformationProcess != nullptr);

                PROCESS_BASIC_INFORMATION pbi;
                ULONG returnLength;
                const NTSTATUS status = NtQueryInformationProcess(hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), &returnLength);
                if (status != 0)
                {
                    return {};
                }

                // ReSharper disable once CppLocalVariableMayBeConst
                PVOID pebAddress = pbi.PebBaseAddress;
                PVOID rtlUserProcParamsAddress;
                if (!ReadProcessMemory(hProcess, static_cast<PCHAR>(pebAddress) + offsetof(PEB, ProcessParameters), &rtlUserProcParamsAddress, sizeof(PVOID), nullptr))
                {
                    return {};
                }

                UNICODE_STRING imagePathName;
                if (!ReadProcessMemory(hProcess, static_cast<PCHAR>(rtlUserProcParamsAddress) + offsetof(RTL_USER_PROCESS_PARAMETERS, ImagePathName), &imagePathName, sizeof(imagePathName), nullptr))
                {
                    return {};
                }

                auto imagePathContents = std::make_unique<WCHAR[]>(imagePathName.Length / 2 + 1);
                if (!ReadProcessMemory(hProcess, imagePathName.Buffer, imagePathContents.get(), imagePathName.Length, nullptr))
                {
                    return {};
                }

                imagePathContents[imagePathName.Length / 2] = 0;
                std::wstring imagePathWStr(imagePathContents.get());
                return imagePathWStr;
            }
        };
    }
}

#endif

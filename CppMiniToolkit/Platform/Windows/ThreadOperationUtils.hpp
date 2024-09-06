#pragma once

#include <string>
#include <Common/BuildConfig.hpp>

#include "Common/ScopedExit.hpp"

#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
#include <windows.h>
#include <cassert>

namespace CppMiniToolkit
{
    namespace Windows
    {
        class ThreadOperationUtils
        {
        public:
            CPP_MINI_TOOLKIT_DECLARE_TOOLKIT_CLASS_TYPE(ThreadOperationUtils);

            // ReSharper disable IdentifierTypo
            typedef enum {  // NOLINT(performance-enum-size)
                ThreadBasicInformation = 0               
            } EThreadInfoClass;
            // ReSharper restore IdentifierTypo

            typedef NTSTATUS(NTAPI* NtQueryInformationThreadType)(HANDLE, EThreadInfoClass, PVOID, ULONG, PULONG);

            static NtQueryInformationThreadType GetNtDllNtQueryInformationThreadFunction()
            {
                static NtQueryInformationThreadType NtQueryInformationThread = nullptr;

                if (NtQueryInformationThread == nullptr)
                {
                    const HMODULE hNtDll = GetModuleHandleW(L"ntdll.dll");
                    if (!hNtDll)
                    {
                        return nullptr;
                    }

                    NtQueryInformationThread = reinterpret_cast<NtQueryInformationThreadType>(GetProcAddress(hNtDll, "NtQueryInformationThread"));  // NOLINT(clang-diagnostic-cast-function-type-strict)

                    assert(NtQueryInformationThread!=nullptr);
                }

                return NtQueryInformationThread;
            }

            typedef HRESULT(WINAPI* GetThreadDescriptionType)(HANDLE, PWSTR*);
            static GetThreadDescriptionType GetKernel32GetThreadDescriptionFunction()
            {
                static GetThreadDescriptionType GetThreadDescription = nullptr;

                if (GetThreadDescription == nullptr)
                {
                    const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
                    if (!hKernel32)
                    {
                        return nullptr;
                    }

                    GetThreadDescription = reinterpret_cast<GetThreadDescriptionType>(GetProcAddress(hKernel32, "GetThreadDescription"));  // NOLINT(clang-diagnostic-cast-function-type-strict)

                    assert(GetThreadDescription != nullptr);
                }

                return GetThreadDescription;
            }

            typedef HRESULT (WINAPI *SetThreadDescriptionType)(HANDLE hThread, PCWSTR lpThreadDescription);
            static SetThreadDescriptionType GetKernel32SetThreadDescriptionFunction()
            {
                static SetThreadDescriptionType SetThreadDescription = nullptr;
                if (SetThreadDescription == nullptr)
                {
                    const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
                    if (!hKernel32)
                    {
                        return nullptr;
                    }

                    SetThreadDescription = reinterpret_cast<SetThreadDescriptionType>(GetProcAddress(hKernel32, "SetThreadDescription"));  // NOLINT(clang-diagnostic-cast-function-type-strict)
                    
                    assert(SetThreadDescription != nullptr);
                }

                return SetThreadDescription;
            }
            
            // Query thread start address
            static LPCVOID GetThreadStartAddress(HANDLE hThread)
            {
                static NtQueryInformationThreadType NtQueryInformationThread = GetNtDllNtQueryInformationThreadFunction();

                assert(NtQueryInformationThread != nullptr);

                PVOID startAddress = nullptr;
                ULONG returnLength;
                const NTSTATUS status = NtQueryInformationThread(
                    hThread,
                    static_cast<EThreadInfoClass>(9 /* ThreadQuerySetWin32StartAddress */),
                    &startAddress, /*NOLINT(bugprone-multi-level-implicit-pointer-conversion)*/
                    sizeof(startAddress),
                    &returnLength);
                
                if (status != 0)
                {
                    return nullptr;
                }

                return startAddress;
            }

            static std::wstring GetThreadName(HANDLE hThread, HANDLE hProcess = nullptr)
            {
                static GetThreadDescriptionType GetThreadDescription = GetKernel32GetThreadDescriptionFunction();
                assert(GetThreadDescription != nullptr);

                PWSTR threadDescription = nullptr;
                const auto Result = GetThreadDescription(hThread, &threadDescription);
                CPP_MINI_TOOLKIT_UNREFERENCED_PARAMETER(Result);
        
                if (threadDescription != nullptr)
                {
                    CPP_MINI_TOOLKIT_SCOPED_EXIT(LocalFree(threadDescription));
                    
                    if(hProcess != nullptr)
                    {
                        SIZE_T bytesRead;     
                        WCHAR buffer[256];
                        if (ReadProcessMemory(hProcess, threadDescription, buffer, sizeof(buffer), &bytesRead))
                        {
                            const std::wstring wThreadDescription(buffer);
                                            
                            return wThreadDescription;
                        }    
                    }
                    else
                    {
                        const std::wstring wThreadDescription(threadDescription);
                        return wThreadDescription;
                    }
                }

                return {};
            }

            static void SetThreadName(DWORD threadId, const WCHAR* threadName)
            {
                const auto hThread = OpenThread(THREAD_SET_LIMITED_INFORMATION, FALSE, threadId);
                if (!hThread)
                {
                    return;
                }

                CPP_MINI_TOOLKIT_SCOPED_EXIT(CloseHandle(hThread));
                
                SetThreadName(hThread, threadName);
            }

            static void SetThreadName(HANDLE hThread, const WCHAR* threadName)
            {
                static SetThreadDescriptionType SetThreadDescription = GetKernel32SetThreadDescriptionFunction();
                assert(SetThreadDescription != nullptr);

                const auto hr = SetThreadDescription(hThread, threadName);
                CPP_MINI_TOOLKIT_UNREFERENCED_PARAMETER(hr);
            }
        };
    }
}

#endif
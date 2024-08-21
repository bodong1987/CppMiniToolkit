#pragma once


#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <windows.h>
#include <Psapi.h>
#include <cassert>
#include <algorithm>

namespace CppMiniToolkit
{
    namespace Windows
    {
        class ProcessMemory
        {
        public:
            ProcessMemory() = delete;
            ~ProcessMemory() = delete;

            struct Section
            {
                LPVOID Start = nullptr;
                LONGLONG Size = 0;

                bool IsValid() const
                {
                    return Start != nullptr;
                }
            };

            static Section FindModuleSection(const HMODULE module, LPCSTR segmentName)
            {
                assert(module != nullptr);
                assert(segmentName != nullptr);

                Section Result;

                MODULEINFO module_info;
                GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(module_info));
                LPVOID Address = module_info.lpBaseOfDll;

                // get module pe
                const auto dosHeader = static_cast<PIMAGE_DOS_HEADER>(Address);
                auto ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(static_cast<BYTE*>(Address) + dosHeader->e_lfanew);

                auto sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
                for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, sectionHeader++)
                {
                    const auto sectionBase = reinterpret_cast<LONGLONG>(Address) + sectionHeader->VirtualAddress;
                    const auto sectionSize = sectionHeader->Misc.VirtualSize;

                    if (strcmp(reinterpret_cast<char*>(sectionHeader->Name), segmentName) == 0)
                    {
                        Result.Start = reinterpret_cast<LPVOID>(sectionBase);
                        Result.Size = sectionSize;

                        return Result;
                    }
                }

                return Result;
            }

            static LPVOID SearchInMemory(const LPCVOID startPos, const LONGLONG size, const LPCVOID signature, const LONGLONG length)
            {
                // ReSharper disable CppCStyleCast
                const BYTE* position = std::search((BYTE*)startPos, (BYTE*)startPos + size, (BYTE*)signature, (BYTE*)signature + length);

                if (position == nullptr || position == (BYTE*)startPos + size)
                {
                    return nullptr;
                }
                // ReSharper restore CppCStyleCast

                return const_cast<BYTE*>(position);
            }

            static LPVOID SearchInSection(const HMODULE module, const LPCSTR segmentName, const LPCVOID signature, const LONGLONG length)
            {
                const auto segment = FindModuleSection(module, segmentName);

                if (segment.IsValid())
                {
                    return SearchInMemory(segment.Start, segment.Size, signature, length);
                }

                return nullptr;
            }

            static bool ReplaceMemory(void* const dest, const LPCVOID source, const LONGLONG length)
            {
                void* TargetAddress = dest;

                DWORD oldProtect;
                if (!VirtualProtect(TargetAddress, length, PAGE_EXECUTE_READWRITE, &oldProtect))
                {
                    return false;
                }

                memcpy(TargetAddress, source, length);

                if (!VirtualProtect(TargetAddress, length, oldProtect, &oldProtect))
                {
                    // error
                    return false;
                }

                // clear
                FlushInstructionCache(GetCurrentProcess(), TargetAddress, length);

                return true;
            }

            static LPVOID PatchMemory(const HMODULE module, const LPCSTR segmentName, const LPCVOID signature, LPCVOID const newBytes, const LONGLONG length)
            {
                const LPVOID pos = SearchInSection(module, segmentName, signature, length); // NOLINT(*-misplaced-const)

                // assert(pos != nullptr);

                // ReSharper disable once CppDFAConstantConditions
                if (pos == nullptr)
                {
                    return nullptr;
                }

                if(!ReplaceMemory(pos, newBytes, length))
                {
                    return nullptr;
                }

                return pos;
            }
        };
    }
}
#endif

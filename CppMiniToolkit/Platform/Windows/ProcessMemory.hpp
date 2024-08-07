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
            ProcessMemory() = delete;
            ~ProcessMemory() = delete;
        public:
            struct Section
            {
                LPVOID Start = nullptr;
                LONGLONG Size = 0;

                bool IsValid() const
                {
                    return Start != 0;
                }
            };

            static Section FindModuleSection(HMODULE module, LPCSTR segmentName)
            {
                assert(module != nullptr);
                assert(segmentName != nullptr);

                Section Result;

                MODULEINFO module_info;
                GetModuleInformation(GetCurrentProcess(), module, &module_info, sizeof(module_info));
                LPVOID Address = module_info.lpBaseOfDll;

                // get moudle pe
                PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(Address);
                PIMAGE_NT_HEADERS ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>((BYTE*)Address + dosHeader->e_lfanew);

                PIMAGE_SECTION_HEADER sectionHeader = IMAGE_FIRST_SECTION(ntHeaders);
                for (int i = 0; i < ntHeaders->FileHeader.NumberOfSections; i++, sectionHeader++)
                {
                    LONGLONG sectionBase = (LONGLONG)Address + sectionHeader->VirtualAddress;
                    LONGLONG sectionSize = sectionHeader->Misc.VirtualSize;

                    if (strcmp((char*)sectionHeader->Name, segmentName) == 0)
                    {
                        Result.Start = (LPVOID)sectionBase;
                        Result.Size = sectionSize;

                        return Result;
                    }
                }

                return Result;
            }

            static LPVOID SearchInMemory(LPCVOID startPos, LONGLONG size, LPCVOID signature, LONGLONG length)
            {
                const BYTE* position = std::search((BYTE*)startPos, (BYTE*)startPos + size, (BYTE*)signature, (BYTE*)signature + length);

                if (position == nullptr || position == (BYTE*)startPos + size)
                {
                    return nullptr;
                }

                return (LPVOID)position;
            }

            static LPVOID SearchInSection(HMODULE module, LPCSTR segmentName, LPCVOID signature, LONGLONG length)
            {
                auto segment = FindModuleSection(module, segmentName);

                if (segment.IsValid())
                {
                    return SearchInMemory(segment.Start, segment.Size, signature, length);
                }

                return nullptr;
            }

            static bool ReplaceMemory(LPVOID dest, LPCVOID source, LONGLONG length)
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

            static LPVOID PatchMemory(HMODULE module, LPCSTR segmentName, LPCVOID signature, LPCVOID newBytes, LONGLONG length)
            {
                LPVOID pos = SearchInSection(module, segmentName, signature, length);

                assert(pos != nullptr);

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

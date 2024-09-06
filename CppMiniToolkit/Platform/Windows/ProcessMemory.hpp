#pragma once

#include <Common/BuildConfig.hpp>

#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
#include <windows.h>
#include <Psapi.h>
#include <cassert>
#include <algorithm>

namespace CppMiniToolkit
{
    namespace PlatformWindows
    {
        class ProcessMemory
        {
        public:
            CPP_MINI_TOOLKIT_DECLARE_TOOLKIT_CLASS_TYPE(ProcessMemory);

            struct Section
            {
                const BYTE* Start = nullptr;
                SIZE_T Size = 0;

                bool IsValid() const
                {
                    return Start != nullptr;
                }
            };

            static Section FindModuleSection(const HMODULE module, const LPCSTR segmentName)
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
                    const auto sectionBase = reinterpret_cast<SIZE_T>(Address) + sectionHeader->VirtualAddress;
                    const auto sectionSize = sectionHeader->Misc.VirtualSize;

                    if (strcmp(reinterpret_cast<char*>(sectionHeader->Name), segmentName) == 0)
                    {
                        Result.Start = reinterpret_cast<const BYTE*>(sectionBase);  // NOLINT(performance-no-int-to-ptr)
                        Result.Size = sectionSize;

                        return Result;
                    }
                }

                return Result;
            }

            static const BYTE* SearchInMemory(const BYTE* startPos, const SIZE_T size, const BYTE* signature, const SIZE_T length)
            {
                // ReSharper disable CppCStyleCast
                const BYTE* position = std::search(startPos, startPos + size, signature, signature + length);

                if (position == nullptr || position == startPos + size)
                {
                    return nullptr;
                }
                // ReSharper restore CppCStyleCast

                return position;
            }

            static const BYTE* FuzzySearchInMemory(const BYTE* startPos, const SIZE_T size, const BYTE* signature, const SIZE_T length, const BYTE wildcard)
            {
                auto compare = [=](BYTE a, BYTE b) -> bool {
                    return a == b || b == wildcard;
                };

                const BYTE* start = startPos;
                const BYTE* end = start + size;
                const BYTE* sig = signature;

                const BYTE* position = std::search(start, end, sig, sig + length, compare);

                if (position == end)
                {
                    return nullptr;
                }

                return const_cast<BYTE*>(position);
            }

            static const BYTE* SearchInSection(const HMODULE module, const LPCSTR segmentName, const BYTE* signature, const SIZE_T length)
            {
                const auto segment = FindModuleSection(module, segmentName);

                if (segment.IsValid())
                {
                    return SearchInMemory(segment.Start, segment.Size, signature, length);
                }

                return nullptr;
            }

            static bool ReplaceMemory(BYTE* const dest, const LPCVOID source, const SIZE_T length)
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

            template <SIZE_T CodeLength>
            static bool ReplaceMemory(BYTE* dest, const BYTE(&codes)[CodeLength])
            {
                return ReplaceMemory(dest, codes, CodeLength);
            }

            static const BYTE* PatchMemory(const HMODULE module, const LPCSTR segmentName, const BYTE* signature, const BYTE* newBytes, const SIZE_T length)
            {
                const BYTE* pos = SearchInSection(module, segmentName, signature, length); // NOLINT(*-misplaced-const)

                // assert(pos != nullptr);

                // ReSharper disable once CppDFAConstantConditions
                if (pos == nullptr)
                {
                    return nullptr;
                }

                if(!ReplaceMemory(const_cast<BYTE*>(pos), newBytes, length))
                {
                    return nullptr;
                }

                return pos;
            }
        };
    }
}
#endif

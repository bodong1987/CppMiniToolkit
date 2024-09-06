#pragma once


#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <Platform/Windows/ProcessMemory.hpp>
#include <vector>

namespace CppMiniToolkit
{
    namespace Windows
    {
        class ProcessPatchUtils  // NOLINT(cppcoreguidelines-special-member-functions)
        {
        public:
            ProcessPatchUtils() = delete;
            ~ProcessPatchUtils() = delete;

            static bool ReplaceAssemblyFunctionAddress(BYTE* codes, const SIZE_T length, const BYTE flag,
                                                       const LPCVOID addressOfFunction)
            {
                const auto FlagPos = std::find(codes, codes + length, flag);
                assert(FlagPos != codes + length);

                if (FlagPos == codes + length)
                {
                    return false;
                }

                const auto FlagDistance = std::distance(codes, FlagPos);

                memcpy(codes + FlagDistance, &addressOfFunction, sizeof(addressOfFunction));  // NOLINT(bugprone-multi-level-implicit-pointer-conversion)

                return true;
            }

            template <SIZE_T CodeLength>
            static bool ReplaceAssemblyFunctionAddress(BYTE (&codes)[CodeLength], BYTE flag,
                                                       const LPCVOID addressOfFunction)
            {
                return ReplaceAssemblyFunctionAddress(codes, CodeLength, flag, addressOfFunction);
            }

            // only support short jump
            struct CommonFunctionHookOptions
            {
                HMODULE ModuleHandle = nullptr;
                LPCSTR SegmentName = nullptr;
                const BYTE* Signature = nullptr;
                SIZE_T SignatureLength = 0;
                CHAR ForwardOffset = 0;
                CHAR PatchOffset = 0;
                CHAR PatchSize = 0;
                BYTE Wildcard = 0xFF;
                BYTE FunctionAddressFlag = 0;
                LPCVOID FunctionPointer = nullptr; // should be assembly function 
                bool FuzzySearchInMemory = false;
            };

            static bool HookCommonFunction(const CommonFunctionHookOptions& options)
            {
                assert(options.ModuleHandle != nullptr);
                assert(options.SegmentName != nullptr);
                assert(options.Signature != nullptr);
                assert(options.FunctionPointer != nullptr);

                const auto CodeSegment = ProcessMemory::FindModuleSection(options.ModuleHandle, options.SegmentName);
                if (!CodeSegment.IsValid())
                {
                    return false;
                }

                const BYTE* AddressOfTargetFunction = options.FuzzySearchInMemory
                                                             ? ProcessMemory::FuzzySearchInMemory(
                                                                 CodeSegment.Start,
                                                                 CodeSegment.Size, options.Signature,
                                                                 options.SignatureLength, options.Wildcard)
                                                             :
                                                                 ProcessMemory::SearchInMemory(
                                                                     CodeSegment.Start, CodeSegment.Size,
                                                                     options.Signature,
                                                                     options.SignatureLength);

                if (!AddressOfTargetFunction)
                {
                    return false;
                }

                std::vector<BYTE> CallAsmFunctionCodes = {
                    0x50, 0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xD0, 0x58
                };

                assert(CallAsmFunctionCodes.size() < 0xFF);
                const BYTE JumpBackOffset = options.ForwardOffset * -1 + options.PatchOffset + options.PatchSize -
                    static_cast<BYTE>(CallAsmFunctionCodes.size()) - 0x4; // NOLINT(clang-diagnostic-implicit-int-conversion)                

                ReplaceAssemblyFunctionAddress(
                    CallAsmFunctionCodes.data(),
                    CallAsmFunctionCodes.size(),
                    options.FunctionAddressFlag,
                    options.FunctionPointer);

                CallAsmFunctionCodes.insert(CallAsmFunctionCodes.end(), AddressOfTargetFunction + options.PatchOffset,
                                            AddressOfTargetFunction + options.PatchOffset + options.PatchSize);

                // short jump
                CallAsmFunctionCodes.push_back(0xEB);
                CallAsmFunctionCodes.push_back(JumpBackOffset);

                if (!ProcessMemory::ReplaceMemory(
                    const_cast<BYTE*>(AddressOfTargetFunction + options.ForwardOffset),                    
                    CallAsmFunctionCodes.data(),
                    CallAsmFunctionCodes.size()))                
                {
                    return false;
                }

                // replace normal code to jump code
                std::vector<BYTE> JumpToCallAsmFunctionCodes = {
                    0xEB, static_cast<BYTE>(options.ForwardOffset - options.PatchOffset - options.PatchSize)
                };
                while (JumpToCallAsmFunctionCodes.size() < static_cast<size_t>(options.PatchSize))
                {
                    JumpToCallAsmFunctionCodes.push_back(0x90); // add nop
                }

                ProcessMemory::ReplaceMemory(
                    const_cast<BYTE*>(AddressOfTargetFunction + options.PatchOffset),                    
                    JumpToCallAsmFunctionCodes.data(),
                    JumpToCallAsmFunctionCodes.size()
                );

                return true;
            }
        };
    }
}

#endif

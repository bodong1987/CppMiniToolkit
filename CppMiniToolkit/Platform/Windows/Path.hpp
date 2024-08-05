#pragma once

#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <string>
#include <windows.h>
#include <tchar.h>
#include <winioctl.h>

#include <Common/ScopedExit.hpp>

namespace CppMiniToolkit
{
    namespace Windows
    {
        class PathUtils
        {
            PathUtils() = delete;
            ~PathUtils() = delete;
        public:
            typedef std::basic_string<TCHAR> StringT;

            static StringT GetEntryPath()
            {
                TCHAR Path[1024];
                GetModuleFileName(::GetModuleHandle(nullptr), Path, _countof(Path));

                return Path;
            }
            
            static StringT GetEntryDirectory()
            {
                auto EntryPath = GetEntryPath();

                auto pos = EntryPath.find_last_of(_T('\\'));

                return pos != StringT::npos ? EntryPath.substr(0, pos) : EntryPath;
            }

            static StringT GetExecuteModulePath()
            {
                HMODULE hModule = nullptr;
                TCHAR Path[1024];
                GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    (LPCTSTR)&GetExecuteModulePath,
                    &hModule
                );

                GetModuleFileName(hModule, Path, _countof(Path));

                return Path;
            }

            static bool IsDriveSSD(const StringT& driveName)
            {
                assert(driveName.size()>=2);

                if (driveName.size() < 2)
                {
                    return false;
                }

                HANDLE hDevice = CreateFile((_T("\\\\.\\") + driveName.substr(0, 2)).c_str(), 0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

                if (hDevice == INVALID_HANDLE_VALUE)
                {
                    return false;
                }

                CPPMINITOOLKIT_SCOPED_EXIT(CloseHandle(hDevice));

                STORAGE_PROPERTY_QUERY query{};
                query.PropertyId = StorageDeviceSeekPenaltyProperty;
                query.QueryType = PropertyStandardQuery;
                DWORD count;
                DEVICE_SEEK_PENALTY_DESCRIPTOR result{};
                if (DeviceIoControl(hDevice, IOCTL_STORAGE_QUERY_PROPERTY,
                    &query, sizeof(query), &result, sizeof(result), &count, nullptr))
                {
                    return !result.IncursSeekPenalty;
                }

                return false;
            }
        };
    }
}
#endif


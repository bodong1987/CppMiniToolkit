#pragma once

#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <string>
#include <tchar.h>
#include <sstream>
#include <iostream>
#include <cassert>
#include <Common/ScopedExit.hpp>
#include <FileSystem/FileSystem.hpp>
#include <Text/Encoding.hpp>

namespace CppMiniToolkit
{
    namespace Windows
    {
        class Envrionment
        {
            Envrionment() = delete;
            ~Envrionment() = delete;
        public:
            typedef std::basic_string<TCHAR> StringT;

            static StringT SearchApplicationInSystemPath(const StringT& appName)
            {
                size_t requiredSize;
                _tgetenv_s(&requiredSize, NULL, 0, _T("PATH"));

                if (requiredSize == 0)
                {
                    return _T("");
                }

                wchar_t* pathValue = new wchar_t[requiredSize];
                CPPMINITOOLKIT_SCOPED_EXIT(delete[] pathValue);

                _tgetenv_s(&requiredSize, pathValue, requiredSize, _T("PATH"));

#if CPPMINITOOLKIT_UNICODE
                std::wstringstream ss(pathValue);
#else
                std::stringstream ss(pathValue);
#endif
                StringT dir;
                while (std::getline(ss, dir, _T(';')))
                {
                    StringT testPath = dir + _T("\\") + appName;

                    if (FileSystem::IsFileExists(testPath.c_str()))
                    {
                        return testPath;
                    }
                }

                return _T("");
            }

            static StringT GetSystemVersion()
            {
                double ret = 0.0;
                NTSTATUS(WINAPI * RtlGetVersion)(LPOSVERSIONINFOEXW);
                OSVERSIONINFOEXW osInfo = { sizeof(OSVERSIONINFOEXW) };

                HMODULE ntHandle = GetModuleHandle(_T("ntdll"));
                assert(ntHandle != nullptr);

                *(FARPROC*)&RtlGetVersion = GetProcAddress(ntHandle, "RtlGetVersion");

                if (NULL != RtlGetVersion)
                {
                    RtlGetVersion(&osInfo);
                    ret = (double)osInfo.dwMajorVersion;
                }

                StringT SystemType = _T("Unknown Windows");

                if (osInfo.dwMajorVersion > 10)
                {
                    SystemType = _T("Windows 11 newer");
                }
                else if (osInfo.dwMajorVersion == 10)
                {
                    if (osInfo.dwBuildNumber >= 21996)
                    {
                        SystemType = _T("Windows 11");
                    }
                    else
                    {
                        SystemType = _T("Windows 10");
                    }
                }
                else if (osInfo.dwMajorVersion == 6)
                {
                    if (osInfo.dwMinorVersion == 3)
                    {
                        SystemType = _T("Windows 8.1");
                    }
                    else if (osInfo.dwMinorVersion == 2)
                    {
                        SystemType = _T("Windows 8");
                    }
                    else if (osInfo.dwMinorVersion == 1)
                    {
                        SystemType = _T("Windows 7");
                    }
                    else if (osInfo.dwMinorVersion == 0)
                    {
                        SystemType = _T("Windows Vista");
                    }
                }
                else if (osInfo.dwMajorVersion == 5)
                {
                    if (osInfo.dwMinorVersion == 1)
                    {
                        SystemType = _T("Windows XP");
                    }
                    else if (osInfo.dwMinorVersion == 0)
                    {
                        SystemType = _T("Windows 2000");
                    }
                }

                BOOL isWow64 = FALSE;
                IsWow64Process(GetCurrentProcess(), &isWow64);

                TCHAR versionString[128];
                _stprintf_s(versionString, _T(" %s [%d.%d.%d]"), (!isWow64 ? _T("x64") : _T("x86")), osInfo.dwMajorVersion, osInfo.dwMinorVersion, osInfo.dwBuildNumber);

                return SystemType + versionString;
            }

            static StringT ExecuteAndGetOutputText(const StringT& appPath, const StringT& appCommandLine)
            {
                std::string Result;
                SECURITY_ATTRIBUTES sa;
                sa.nLength = sizeof(sa);
                sa.lpSecurityDescriptor = NULL;
                sa.bInheritHandle = TRUE;

                HANDLE hRead, hWrite;
                if (!CreatePipe(&hRead, &hWrite, &sa, 0))
                {
                    return _T("");
                }

                STARTUPINFO si;
                ZeroMemory(&si, sizeof(si));
                si.cb = sizeof(si);
                si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
                si.wShowWindow = SW_HIDE;
                si.hStdOutput = hWrite;
                si.hStdError = hWrite;

                PROCESS_INFORMATION pi;
                ZeroMemory(&pi, sizeof(pi));

                StringT cmd = appPath + _T(" ") + appCommandLine;

                if (!CreateProcess(NULL, (LPTSTR)cmd.c_str(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
                {
                    CloseHandle(hWrite);
                    CloseHandle(hRead);

                    return _T("");
                }

                CloseHandle(hWrite);

                std::vector<char> buffer(4096);
                DWORD bytesRead = 0;
                while (ReadFile(hRead, buffer.data(), (DWORD)buffer.size(), &bytesRead, NULL))
                {
                    Result.append(buffer.data(), bytesRead);

                    if (bytesRead <= 0)
                    {
                        break;
                    }
                }

                CloseHandle(hRead);
                CloseHandle(pi.hThread);
                CloseHandle(pi.hProcess);

                #if CPPMINITOOLKIT_UNICODE
                auto str = Encoding::ANSIToUTF16(Result.c_str());
                return (LPCWSTR)str.c_str();
                #else
                return Result;
                #endif
            }

            struct FileExtraInfo
            {
                StringT FileVersion;
                StringT ProductVersion;
                StringT FileDescription;
                StringT FileCopyright;
                uint64_t FileSize = 0;
                StringT LastModifyTime;
            };

            static FileExtraInfo GetFileExtraInfo(const StringT& filePath)
            {
                FileExtraInfo Result;

                // get version data
                DWORD handle = 0;
                DWORD size = GetFileVersionInfoSize(filePath.c_str(), &handle);

                if (size > 0)
                {
                    uint8_t* versionInfo = new uint8_t[size];
                    CPPMINITOOLKIT_SCOPED_EXIT(delete[] versionInfo);

                    if (GetFileVersionInfo(filePath.c_str(), 0, size, versionInfo) != FALSE)
                    {
                        LPVOID buffer = nullptr;
                        VS_FIXEDFILEINFO* fileInfo = nullptr;
                        UINT len = 0;

                        if (VerQueryValue(versionInfo, _T("\\"), (LPVOID*)&fileInfo, &len) != FALSE && fileInfo != nullptr)
                        {
                            TCHAR szVersion[128];
                            _stprintf_s(szVersion, _T("%u.%u.%u.%u"), HIWORD(fileInfo->dwFileVersionMS), LOWORD(fileInfo->dwFileVersionMS), HIWORD(fileInfo->dwFileVersionLS), LOWORD(fileInfo->dwFileVersionLS));

                            Result.FileVersion = szVersion;
                        }

                        if (VerQueryValue(versionInfo, _T("\\VarFileInfo\\Translation"), &buffer, &len) != FALSE && buffer != nullptr)
                        {
                            WORD* langInfo = (WORD*)buffer;
                            for (UINT i = 0; i < len / sizeof(WORD) / 2; i++)
                            {
                                WORD lang = langInfo[i * 2];
                                WORD charset = langInfo[i * 2 + 1];

                                TCHAR blockName[128] = { 0 };
                                LPVOID lbuffer = nullptr;

                                // 040904b0 english
                                // 080404b0 chinese-simple
                                if (Result.ProductVersion.empty())
                                {
                                    _stprintf_s(blockName, _T("\\StringFileInfo\\%04hx%04hx\\ProductVersion"), lang, charset);

                                    if (VerQueryValue(versionInfo, blockName, &lbuffer, &len) != FALSE && lbuffer != nullptr)
                                    {
                                        Result.ProductVersion = (TCHAR*)lbuffer;
                                    }
                                }

                                if (Result.FileDescription.empty())
                                {
                                    _stprintf_s(blockName, _T("\\StringFileInfo\\%04hx%04hx\\FileDescription"), lang, charset);

                                    if (VerQueryValue(versionInfo, blockName, &lbuffer, &len) != FALSE && lbuffer != nullptr)
                                    {
                                        Result.FileDescription = (TCHAR*)lbuffer;
                                    }
                                }

                                if (Result.FileCopyright.empty())
                                {
                                    _stprintf_s(blockName, _T("\\StringFileInfo\\%04hx%04hx\\LegalCopyright"), lang, charset);

                                    if (VerQueryValue(versionInfo, blockName, &lbuffer, &len) != FALSE && lbuffer != nullptr)
                                    {
                                        Result.FileCopyright = (TCHAR*)lbuffer;
                                    }
                                }
                            }
                        }
                    }
                }

                WIN32_FILE_ATTRIBUTE_DATA fileData;
                if (GetFileAttributesEx(filePath.c_str(), GetFileExInfoStandard, &fileData) != FALSE)
                {
                    LARGE_INTEGER fileSize;
                    fileSize.HighPart = fileData.nFileSizeHigh;
                    fileSize.LowPart = fileData.nFileSizeLow;

                    Result.FileSize = *(uint64_t*)&fileSize;

                    FILETIME localFileTime;
                    if (FileTimeToLocalFileTime(&fileData.ftLastWriteTime, &localFileTime) != FALSE)
                    {
                        SYSTEMTIME systemTime;
                        if (FileTimeToSystemTime(&localFileTime, &systemTime) != FALSE)
                        {
                            TCHAR date[24], time[24];
                            GetDateFormat(LOCALE_USER_DEFAULT, 0, &systemTime, _T("yyyy/M/d"), date, 20);
                            GetTimeFormat(LOCALE_USER_DEFAULT, 0, &systemTime, _T("H:mm"), time, 20);

                            Result.LastModifyTime = StringT(date) + _T(" ") + time;
                        }
                    }
                }

                return Result;
            }
        };
    }
}
#endif

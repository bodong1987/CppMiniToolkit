#pragma once

#if !CPPMINITOOLKIT_PLATFORM_WINDOWS
#error "This header file is provide for windows only"
#endif

#if CPPMINITOOLKIT_PLATFORM_WINDOWS

#include <windows.h>
#include <string>

namespace CppMiniToolkit
{
    namespace PlatformWindows
    {
        // ReSharper disable CppRedundantParentheses
        class FileSystemWindows
        {
        public:
            static bool IsExists(LPCSTR const path)
            {
                const DWORD attrs = GetFileAttributesA(path);
                return attrs != INVALID_FILE_ATTRIBUTES;
            }

            static bool IsExists(LPCWSTR const path)
            {
                const DWORD attrs = GetFileAttributesW(path);
                return attrs != INVALID_FILE_ATTRIBUTES;
            }

            static bool IsFileExists(LPCSTR const path)
            {
                const DWORD attrs = GetFileAttributesA(path);
                return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
            }
            
            static bool IsFileExists(LPCWSTR const path)
            {
                const DWORD attrs = GetFileAttributesW(path);
                return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
            }

            static bool IsDirectoryExists(LPCSTR const directoryPath)
            {
                const DWORD dwAttrib = GetFileAttributesA(directoryPath);

                return dwAttrib != INVALID_FILE_ATTRIBUTES && (((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0));
            }

            static bool IsDirectoryExists(LPCWSTR const directoryPath)
            {
                const DWORD dwAttrib = GetFileAttributesW(directoryPath);

                return dwAttrib != INVALID_FILE_ATTRIBUTES && (((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0));
            }

            static bool IsDirectoryEmpty(LPCSTR const directoryPath)
            {
                if (!IsDirectoryExists(directoryPath))
                {
                    return false;
                }

                int count = 0;
                auto visitor = [&](LPCSTR, bool) 
                {
                    ++count;
                    return false;
                };

                WalkThoughDirectory(directoryPath, visitor, false, true);

                return count <= 0;
            }

            static bool IsDirectoryEmpty(LPCWSTR const directoryPath)
            {
                if (!IsDirectoryExists(directoryPath))
                {
                    return false;
                }

                int count = 0;
                auto visitor = [&](LPCWSTR, bool)
                    {
                        ++count;
                        return false;
                    };

                WalkThoughDirectory(directoryPath, visitor, false, true);

                return count <= 0;
            }

            static bool CreateDirectories(LPCSTR const directoryPath, const bool recursively) // NOLINT(*-no-recursion)
            {
                if (directoryPath == nullptr)
                {
                    return false;
                }

                static const std::string separators("\\/");
                const std::string directory = directoryPath;

                // If the specified directory name doesn't exist, do our thing
                const DWORD fileAttributes = ::GetFileAttributesA(directory.c_str());

                if (fileAttributes == INVALID_FILE_ATTRIBUTES)
                {
                    if (recursively)
                    {
                        // Recursively do it all again for the parent directory, if any
                        const SIZE_T slashIndex = directory.find_last_of(separators);
                        if (slashIndex != std::wstring::npos)
                        {
                            if (!CreateDirectories(directory.substr(0, slashIndex).c_str(), recursively))
                            {
                                return false;
                            }
                        }
                    }

                    // Create the last directory on the path (the recursive calls will have taken
                    // care of the parent directories by now)
                    const BOOL result = ::CreateDirectoryA(directory.c_str(), nullptr);

                    if (!result)
                    {                        
                        return false;
                    }

                    return true;
                }
                // Specified directory name already exists as a file or directory

                const bool isDirectoryOrJunction =
                    ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                    ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

                if (!isDirectoryOrJunction)
                {
                    // there is already have a file with the same name
                    return false;
                }

                return true;
            }

            static bool CreateDirectories(LPCWSTR const directoryPath, const bool recursively) // NOLINT(*-no-recursion)
            {
                if (directoryPath == nullptr)
                {
                    return false;
                }

                static const std::wstring separators(L"\\/");
                const std::wstring directory = directoryPath;

                // If the specified directory name doesn't exist, do our thing
                const DWORD fileAttributes = ::GetFileAttributesW(directory.c_str());

                if (fileAttributes == INVALID_FILE_ATTRIBUTES)
                {
                    if (recursively)
                    {
                        // Recursively do it all again for the parent directory, if any
                        const SIZE_T slashIndex = directory.find_last_of(separators);
                        if (slashIndex != std::wstring::npos)
                        {
                            if (!CreateDirectories(directory.substr(0, slashIndex).c_str(), recursively))
                            {
                                return false;
                            }
                        }
                    }

                    // Create the last directory on the path (the recursive calls will have taken
                    // care of the parent directories by now)
                    const BOOL result = ::CreateDirectoryW(directory.c_str(), nullptr);

                    if (!result)
                    {
                        return false;
                    }

                    return true;
                }
                // Specified directory name already exists as a file or directory

                const bool isDirectoryOrJunction =
                    ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                    ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

                if (!isDirectoryOrJunction)
                {
                    // there is already have a file with the same name
                    return false;
                }

                return true;
            }

            static bool DeleteDirectories(LPCSTR const directoryPath, const bool recursively) // NOLINT(*-no-recursion)
            {
                // subdirectories have been found
                // Handle to directory
                std::string      strFilePath;                 // Filepath                
                WIN32_FIND_DATAA FileInformation;             // File information

                const std::string      directory = directoryPath;

                const std::string strPattern = std::string(directoryPath) + "\\*.*";

                const HANDLE hFile = ::FindFirstFileA(strPattern.c_str(), &FileInformation); // NOLINT(*-misplaced-const)

                if (hFile != INVALID_HANDLE_VALUE)
                {
                    bool bSubdirectory = false;
                    do
                    {
                        if (FileInformation.cFileName[0] != '.')
                        {
                            strFilePath.erase();
                            strFilePath = directory + "\\" + FileInformation.cFileName;

                            if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                            {
                                if (recursively)
                                {
                                    // Delete subdirectory
                                    const auto iRC = DeleteDirectories(strFilePath.c_str(), recursively);
                                    if (!iRC)
                                    {
                                        return iRC;
                                    }
                                }
                                else
                                {
                                    bSubdirectory = true;
                                }
                            }
                            else
                            {
                                // Set file attributes
                                if (::SetFileAttributesA(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
                                {
                                    // remove other tags: readonly
                                    return false;
                                }

                                // Delete file
                                if (::DeleteFileA(strFilePath.c_str()) == FALSE)
                                {
                                    // Failed delete files.
                                    return false;
                                }
                            }
                        }
                    } while (::FindNextFileA(hFile, &FileInformation) == TRUE);

                    // Close handle
                    ::FindClose(hFile);

                    const DWORD dwError = GetLastError();
                    if (dwError != ERROR_NO_MORE_FILES)
                    {
                        return false;
                    }
                    if (!bSubdirectory)
                    {
                        // Set directory attributes
                        if (::SetFileAttributesA(directory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
                        {
                            return false;
                        }

                        // Delete directory
                        if (::RemoveDirectoryA(directory.c_str()) == FALSE)
                        {
                            return false;
                        }
                    }
                }

                return true;
            }

            static bool DeleteDirectories(LPCWSTR directoryPath, bool recursively) // NOLINT(*-no-recursion)
            {
                // subdirectories have been found
                // Handle to directory
                std::wstring      strFilePath;                 // Filepath                
                WIN32_FIND_DATAW  FileInformation;             // File information

                const std::wstring      directory = directoryPath;

                const std::wstring strPattern = std::wstring(directoryPath) + L"\\*.*";

                const HANDLE hFile = ::FindFirstFileW(strPattern.c_str(), &FileInformation); // NOLINT(*-misplaced-const)

                if (hFile != INVALID_HANDLE_VALUE)
                {
                    bool bSubdirectory = false;
                    do
                    {
                        if (FileInformation.cFileName[0] != L'.')
                        {
                            strFilePath.erase();
                            strFilePath = directory + L"\\" + FileInformation.cFileName;

                            if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                            {
                                if (recursively)
                                {
                                    // Delete subdirectory
                                    const auto iRC = DeleteDirectories(strFilePath.c_str(), recursively);
                                    if (!iRC)
                                    {
                                        return iRC;
                                    }
                                }
                                else
                                {
                                    bSubdirectory = true;
                                }
                            }
                            else
                            {
                                // Set file attributes
                                if (::SetFileAttributesW(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
                                {
                                    // remove other tags: readonly
                                    return false;
                                }

                                // Delete file
                                if (::DeleteFileW(strFilePath.c_str()) == FALSE)
                                {
                                    // Failed delete files.
                                    return false;
                                }
                            }
                        }
                    } while (::FindNextFileW(hFile, &FileInformation) == TRUE);

                    // Close handle
                    ::FindClose(hFile);

                    const DWORD dwError = GetLastError();
                    if (dwError != ERROR_NO_MORE_FILES)
                    {
                        return false;
                    }
                    if (!bSubdirectory)
                    {
                        // Set directory attributes
                        if (::SetFileAttributesW(directory.c_str(), FILE_ATTRIBUTE_NORMAL) == FALSE)
                        {
                            return false;
                        }

                        // Delete directory
                        if (::RemoveDirectoryW(directory.c_str()) == FALSE)
                        {
                            return false;
                        }
                    }
                }

                return true;
            }

            static bool DeleteSingleFile(LPCSTR const path)
            {
                return ::DeleteFileA(path) != FALSE;
            }

            static bool DeleteSingleFile(LPCWSTR const path)
            {
                return ::DeleteFileW(path) != FALSE;
            }

            static bool CopySingleFile(LPCSTR const from, LPCSTR const to, const bool forceOverride)
            {
                return ::CopyFileA(from, to, forceOverride?FALSE:TRUE) != FALSE;
            }

            static bool CopySingleFile(const LPCWSTR from, const LPCWSTR to, const bool forceOverride)
            {
                return ::CopyFileW(from, to, forceOverride ? FALSE : TRUE) != FALSE;
            }

            static bool MoveSingleFile(const LPCSTR from, const LPCSTR to, const bool forceOverride)
            {
                if (forceOverride && IsFileExists(to))
                {
                    DeleteSingleFile(to);
                }

                return ::MoveFileA(from, to) != FALSE;
            }

            static bool MoveSingleFile(const LPCWSTR from, const LPCWSTR to, const bool forceOverride)
            {
                if (forceOverride && IsFileExists(to))
                {
                    DeleteSingleFile(to);
                }

                return ::MoveFileW(from, to) != FALSE;
            }

            static void CopyDirectoryRecursively(const LPCSTR source, const LPCSTR destination) // NOLINT(*-no-recursion)
            {
                WIN32_FIND_DATAA info;
                char temp[1024] = { 0 };
                char temp_1[1024] = { 0 };

                char temp_src[1024] = { 0 };
                char temp_dest[1024] = { 0 };

                strcpy_s(temp_1, source);
                strcat_s(temp_1, "\\*.*");

                CreateDirectories(destination, true);

                const HANDLE hwnd = FindFirstFileA(temp_1, &info); // NOLINT(*-misplaced-const)
                do
                {
                    if (!strcmp(info.cFileName, "."))
                    {
                        continue;
                    }

                    if (!strcmp(info.cFileName, ".."))
                    {
                        continue;
                    }

                    strcpy_s(temp_src, source);
                    strcat_s(temp_src, "\\");
                    strcat_s(temp_src, info.cFileName);

                    if (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
                    {
                        strcpy_s(temp_dest, destination);
                        strcat_s(temp_dest, "\\");
                        strcat_s(temp_dest, info.cFileName);

                        CopyDirectoryRecursively(temp_src, temp_dest);
                    }
                    else
                    {
                        strcpy_s(temp, destination);
                        strcat_s(temp, "\\");
                        strcat_s(temp, info.cFileName);

                        ::CopyFileA(temp_src, temp, FALSE);
                    }
                } while (FindNextFileA(hwnd, &info));

                FindClose(hwnd);
            }

            static void CopyDirectoryRecursively(const LPCWSTR source, const LPCWSTR destination) // NOLINT(*-no-recursion)
            {
                WIN32_FIND_DATAW info;
                wchar_t temp[1024] = { 0 };
                wchar_t temp_1[1024] = { 0 };

                wchar_t temp_src[1024] = { 0 };
                wchar_t temp_dest[1024] = { 0 };

                wcscpy_s(temp_1, source);
                wcscat_s(temp_1, L"\\*.*");

                CreateDirectories(destination, true);

                const HANDLE hwnd = FindFirstFileW(temp_1, &info); // NOLINT(*-misplaced-const)
                do
                {
                    if (!wcscmp(info.cFileName, L"."))
                    {
                        continue;
                    }

                    if (!wcscmp(info.cFileName, L".."))
                    {
                        continue;
                    }

                    wcscpy_s(temp_src, source);
                    wcscat_s(temp_src, L"\\");
                    wcscat_s(temp_src, info.cFileName);

                    if (info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
                    {
                        wcscpy_s(temp_dest, destination);
                        wcscat_s(temp_dest, L"\\");
                        wcscat_s(temp_dest, info.cFileName);

                        CopyDirectoryRecursively(temp_src, temp_dest);
                    }
                    else
                    {
                        wcscpy_s(temp, destination);
                        wcscat_s(temp, L"\\");
                        wcscat_s(temp, info.cFileName);

                        ::CopyFileW(temp_src, temp, FALSE);
                    }
                } while (FindNextFileW(hwnd, &info));

                FindClose(hwnd);
            }


            static void WalkThoughDirectory(const std::string& directory, const std::function<bool (const char*, bool)>& visitor, const bool recursively, const bool includeDirectories) // NOLINT(*-no-recursion)
            {
                WIN32_FIND_DATAA win32_find_dataa;
                const HANDLE hFind = FindFirstFileA((directory + "\\*").c_str(), &win32_find_dataa); // NOLINT(*-misplaced-const)
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (strcmp(win32_find_dataa.cFileName, ".") == 0 ||
                            strcmp(win32_find_dataa.cFileName, "..") == 0)
                        {
                            continue;
                        }

                        // this is directory
                        if ((win32_find_dataa.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                        {
                            if (includeDirectories)
                            {
                                if (!visitor((directory + "\\" + win32_find_dataa.cFileName).c_str(), true))
                                {
                                    break;
                                }
                            }

                            if (recursively)
                            {
                                WalkThoughDirectory(directory + "\\" + win32_find_dataa.cFileName, visitor, recursively, includeDirectories);
                            }
                        }
                        else
                        {
                            if (!visitor((directory + "\\" + win32_find_dataa.cFileName).c_str(), false))
                            {
                                break;
                            }
                        }
                    } while (FindNextFileA(hFind, &win32_find_dataa));

                    FindClose(hFind);
                }
            }

            static void WalkThoughDirectory(const std::wstring& directory, const std::function<bool(const wchar_t*, bool)>& visitor, const bool recursively, const bool includeDirectories) // NOLINT(*-no-recursion)
            {
                WIN32_FIND_DATAW win32_find_dataw;
                const HANDLE hFind = FindFirstFileW((directory + L"\\*").c_str(), &win32_find_dataw); // NOLINT(*-misplaced-const)
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (wcscmp(win32_find_dataw.cFileName, L".") == 0 ||
                            wcscmp(win32_find_dataw.cFileName, L"..") == 0)
                        {
                            continue;
                        }

                        // this is directory
                        if ((win32_find_dataw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                        {
                            if (includeDirectories)
                            {
                                if (!visitor((directory + L"\\" + win32_find_dataw.cFileName).c_str(), true))
                                {
                                    break;
                                }
                            }

                            if (recursively)
                            {
                                WalkThoughDirectory(directory + L"\\" + win32_find_dataw.cFileName, visitor, recursively, includeDirectories);
                            }
                        }
                        else
                        {
                            if (!visitor((directory + L"\\" + win32_find_dataw.cFileName).c_str(), false))
                            {
                                break;
                            }
                        }
                    } while (FindNextFileW(hFind, &win32_find_dataw));

                    FindClose(hFind);
                }
            }
        };
    }

    typedef PlatformWindows::FileSystemWindows FileSystemDetails;
}
#endif
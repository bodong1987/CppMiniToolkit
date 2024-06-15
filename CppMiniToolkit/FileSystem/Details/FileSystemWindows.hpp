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
        class FileSystemWindows
        {
        public:
            static bool IsExists(LPCSTR path)
            {
                DWORD attrs = GetFileAttributesA(path);
                return attrs != INVALID_FILE_ATTRIBUTES;
            }

            static bool IsExists(LPCWSTR path)
            {
                DWORD attrs = GetFileAttributesW(path);
                return attrs != INVALID_FILE_ATTRIBUTES;
            }

            static bool IsFileExists(LPCSTR path)
            {
                DWORD attrs = GetFileAttributesA(path);
                return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
            }
            
            static bool IsFileExists(LPCWSTR path)
            {
                DWORD attrs = GetFileAttributesW(path);
                return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
            }

            static bool IsDirectoryExists(LPCSTR directoryPath)
            {
                DWORD dwAttrib = GetFileAttributesA(directoryPath);

                return dwAttrib != INVALID_FILE_ATTRIBUTES && (((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0));
            }

            static bool IsDirectoryExists(LPCWSTR directoryPath)
            {
                DWORD dwAttrib = GetFileAttributesW(directoryPath);

                return dwAttrib != INVALID_FILE_ATTRIBUTES && (((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) != 0));
            }

            static bool IsDirectoryEmpty(LPCSTR directoryPath)
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

            static bool IsDirectoryEmpty(LPCWSTR directoryPath)
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

            static bool CreateDirectories(LPCSTR directoryPath, bool recursively)
            {
                if (directoryPath == nullptr)
                {
                    return false;
                }

                static const std::string separators("\\/");
                std::string directory = directoryPath;

                // If the specified directory name doesn't exist, do our thing
                DWORD fileAttributes = ::GetFileAttributesA(directory.c_str());

                if (fileAttributes == INVALID_FILE_ATTRIBUTES)
                {
                    if (recursively)
                    {
                        // Recursively do it all again for the parent directory, if any
                        SIZE_T slashIndex = directory.find_last_of(separators);
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
                    BOOL result = ::CreateDirectoryA(directory.c_str(), nullptr);

                    if (!result)
                    {                        
                        return false;
                    }

                    return true;
                }
                else
                { // Specified directory name already exists as a file or directory

                    bool isDirectoryOrJunction =
                        ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                        ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

                    if (!isDirectoryOrJunction)
                    {
                        // there is already have a file with the same name
                        return false;
                    }

                    return true;
                }
            }

            static bool CreateDirectories(LPCWSTR directoryPath, bool recursively)
            {
                if (directoryPath == nullptr)
                {
                    return false;
                }

                static const std::wstring separators(L"\\/");
                std::wstring directory = directoryPath;

                // If the specified directory name doesn't exist, do our thing
                DWORD fileAttributes = ::GetFileAttributesW(directory.c_str());

                if (fileAttributes == INVALID_FILE_ATTRIBUTES)
                {
                    if (recursively)
                    {
                        // Recursively do it all again for the parent directory, if any
                        SIZE_T slashIndex = directory.find_last_of(separators);
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
                    BOOL result = ::CreateDirectoryW(directory.c_str(), nullptr);

                    if (!result)
                    {
                        return false;
                    }

                    return true;
                }
                else
                { // Specified directory name already exists as a file or directory

                    bool isDirectoryOrJunction =
                        ((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
                        ((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

                    if (!isDirectoryOrJunction)
                    {
                        // there is already have a file with the same name
                        return false;
                    }

                    return true;
                }
            }

            static bool DeleteDirectories(LPCSTR directoryPath, bool recursively)
            {                
                bool             bSubdirectory = false;       // Flag, indicating whether

                // subdirectories have been found
                HANDLE           hFile;                       // Handle to directory
                std::string      strFilePath;                 // Filepath                
                WIN32_FIND_DATAA FileInformation;             // File information

                std::string      directory = directoryPath;

                std::string strPattern = std::string(directoryPath) + "\\*.*";

                hFile = ::FindFirstFileA(strPattern.c_str(), &FileInformation);

                if (hFile != INVALID_HANDLE_VALUE)
                {
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
                                    auto iRC = DeleteDirectories(strFilePath.c_str(), recursively);
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

                    DWORD dwError = GetLastError();
                    if (dwError != ERROR_NO_MORE_FILES)
                    {
                        return false;
                    }
                    else
                    {
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
                }

                return true;
            }

            static bool DeleteDirectories(LPCWSTR directoryPath, bool recursively)
            {
                bool              bSubdirectory = false;       // Flag, indicating whether

                // subdirectories have been found
                HANDLE            hFile;                       // Handle to directory
                std::wstring      strFilePath;                 // Filepath                
                WIN32_FIND_DATAW  FileInformation;             // File information

                std::wstring      directory = directoryPath;

                std::wstring strPattern = std::wstring(directoryPath) + L"\\*.*";

                hFile = ::FindFirstFileW(strPattern.c_str(), &FileInformation);

                if (hFile != INVALID_HANDLE_VALUE)
                {
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
                                    auto iRC = DeleteDirectories(strFilePath.c_str(), recursively);
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

                    DWORD dwError = GetLastError();
                    if (dwError != ERROR_NO_MORE_FILES)
                    {
                        return false;
                    }
                    else
                    {
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
                }

                return true;
            }

            static bool DeleteSingleFile(LPCSTR path)
            {
                return ::DeleteFileA(path) != FALSE;
            }

            static bool DeleteSingleFile(LPCWSTR path)
            {
                return ::DeleteFileW(path) != FALSE;
            }

            static bool CopySingleFile(LPCSTR from, LPCSTR to, bool forceOverride)
            {
                return ::CopyFileA(from, to, forceOverride?FALSE:TRUE) != FALSE;
            }

            static bool CopySingleFile(LPCWSTR from, LPCWSTR to, bool forceOverride)
            {
                return ::CopyFileW(from, to, forceOverride ? FALSE : TRUE) != FALSE;
            }

            static bool MoveSingleFile(LPCSTR from, LPCSTR to, bool forceOverride)
            {
                if (forceOverride && IsFileExists(to))
                {
                    DeleteSingleFile(to);
                }

                return ::MoveFileA(from, to) != FALSE;
            }

            static bool MoveSingleFile(LPCWSTR from, LPCWSTR to, bool forceOverride)
            {
                if (forceOverride && IsFileExists(to))
                {
                    DeleteSingleFile(to);
                }

                return ::MoveFileW(from, to) != FALSE;
            }

            static void CopyDirectoryRecusively(LPCSTR source, LPCSTR destination)
            {
                WIN32_FIND_DATAA info;
                HANDLE hwnd;
                char temp[1024] = { 0 };
                char temp_1[1024] = { 0 };

                char temp_src[1024] = { 0 };
                char temp_dest[1024] = { 0 };

                strcpy_s(temp_1, source);
                strcat_s(temp_1, "\\*.*");

                CreateDirectories(destination, true);

                hwnd = FindFirstFileA(temp_1, &info);
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

                        CopyDirectoryRecusively(temp_src, temp_dest);
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

            static void CopyDirectoryRecusively(LPCWSTR source, LPCWSTR destination)
            {
                WIN32_FIND_DATAW info;
                HANDLE hwnd;
                wchar_t temp[1024] = { 0 };
                wchar_t temp_1[1024] = { 0 };

                wchar_t temp_src[1024] = { 0 };
                wchar_t temp_dest[1024] = { 0 };

                wcscpy_s(temp_1, source);
                wcscat_s(temp_1, L"\\*.*");

                CreateDirectories(destination, true);

                hwnd = FindFirstFileW(temp_1, &info);
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

                        CopyDirectoryRecusively(temp_src, temp_dest);
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


            static void WalkThoughDirectory(std::string directory, std::function<bool (const char*, bool)> visitor, bool recursively, bool includeDirectories)
            {
                WIN32_FIND_DATAA findfiledata;
                HANDLE hFind = FindFirstFileA((directory + "\\*").c_str(), &findfiledata);
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (strcmp(findfiledata.cFileName, ".") == 0 ||
                            strcmp(findfiledata.cFileName, "..") == 0)
                        {
                            continue;
                        }

                        // this is directory
                        if ((findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                        {
                            if (includeDirectories)
                            {
                                if (!visitor((directory + "\\" + findfiledata.cFileName).c_str(), true))
                                {
                                    break;
                                }
                            }

                            if (recursively)
                            {
                                WalkThoughDirectory(directory + "\\" + findfiledata.cFileName, visitor, recursively, includeDirectories);
                            }
                        }
                        else
                        {
                            if (!visitor((directory + "\\" + findfiledata.cFileName).c_str(), false))
                            {
                                break;
                            }
                        }
                    } while (FindNextFileA(hFind, &findfiledata));

                    FindClose(hFind);
                }
            }

            static void WalkThoughDirectory(std::wstring directory, std::function<bool(const wchar_t*, bool)> visitor, bool recursively, bool includeDirectories)
            {
                WIN32_FIND_DATAW findfiledata;
                HANDLE hFind = FindFirstFileW((directory + L"\\*").c_str(), &findfiledata);
                if (hFind != INVALID_HANDLE_VALUE)
                {
                    do
                    {
                        if (wcscmp(findfiledata.cFileName, L".") == 0 ||
                            wcscmp(findfiledata.cFileName, L"..") == 0)
                        {
                            continue;
                        }

                        // this is directory
                        if ((findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
                        {
                            if (includeDirectories)
                            {
                                if (!visitor((directory + L"\\" + findfiledata.cFileName).c_str(), true))
                                {
                                    break;
                                }
                            }

                            if (recursively)
                            {
                                WalkThoughDirectory(directory + L"\\" + findfiledata.cFileName, visitor, recursively, includeDirectories);
                            }
                        }
                        else
                        {
                            if (!visitor((directory + L"\\" + findfiledata.cFileName).c_str(), false))
                            {
                                break;
                            }
                        }
                    } while (FindNextFileW(hFind, &findfiledata));

                    FindClose(hFind);
                }
            }
        };
    }

    typedef PlatformWindows::FileSystemWindows FileSystemDetails;
}
#endif
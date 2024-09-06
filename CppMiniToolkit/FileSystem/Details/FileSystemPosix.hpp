#pragma once
#include <string>

#if !CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <functional>

namespace CppMiniToolkit
{
    namespace PlatformPosix
    {
        class FileSystemPosix
        {
        public:
            static bool IsExists(const char* path)
            {
                struct stat info;
                return stat(path, &info) == 0;
            }

            static bool IsFileExists(const char* path)
            {
                struct stat info;
                return stat(path, &info) == 0 && S_ISREG(info.st_mode);
            }

            static bool IsDirectoryExists(const char* directoryPath)
            {
                struct stat info;
                return stat(directoryPath, &info) == 0 && S_ISDIR(info.st_mode);
            }

            static bool IsDirectoryEmpty(const char* directoryPath)
            {
                DIR* dir = opendir(directoryPath);
                if (!dir)
                {
                    return true;  // If we can't open the directory, assume it's empty
                }

                bool isEmpty = true;
                dirent* entry;
                while ((entry = readdir(dir)) != nullptr)
                {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                    {
                        isEmpty = false;
                        break;
                    }
                }

                closedir(dir);
                return isEmpty;
            }

            static bool CreateDirectories(const char* directoryPath, bool recursively)
            {
                if (!recursively)
                {
                    return mkdir(directoryPath, 0755) == 0;
                }

                // Recursively create parent directories
                std::string path(directoryPath);
                size_t slashPos = path.find('/');
                while (slashPos != std::string::npos)
                {
                    std::string subPath = path.substr(0, slashPos);
                    if (subPath.empty())
                    {
                        // Skip leading slash
                        slashPos = path.find('/', slashPos + 1);
                        continue;
                    }

                    if (!IsDirectoryExists(subPath.c_str()))
                    {
                        if (mkdir(subPath.c_str(), 0755) != 0)
                        {
                            return false;
                        }
                    }

                    slashPos = path.find('/', slashPos + 1);
                }

                return mkdir(directoryPath, 0755) == 0;
            }

            static bool DeleteDirectories(const char* directoryPath, bool recursively)
            {
                if (!recursively)
                {
                    return rmdir(directoryPath) == 0;
                }

                // Recursively delete subdirectories and files
                DIR* dir = opendir(directoryPath);
                if (!dir)
                {
                    return false;
                }

                bool success = true;
                dirent* entry;
                while ((entry = readdir(dir)) != nullptr)
                {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    {
                        continue;
                    }

                    std::string fullPath(directoryPath);
                    fullPath += "/";
                    fullPath += entry->d_name;

                    if (entry->d_type == DT_DIR)
                    {
                        success = success && DeleteDirectories(fullPath.c_str(), true);
                    }
                    else
                    {
                        success = success && DeleteSingleFile(fullPath.c_str());
                    }
                }

                closedir(dir);
                return success && rmdir(directoryPath) == 0;
            }

            static bool DeleteSingleFile(const char* path)
            {
                return unlink(path) == 0;
            }

            static bool CopySingleFile(const char* from, const char* to, bool forceOverride)
            {
                if (!forceOverride && IsFileExists(to))
                {
                    return false;
                }

                std::ifstream src(from, std::ios::binary);
                std::ofstream dst(to, std::ios::binary);
                dst << src.rdbuf();

                return src && dst;
            }

            static bool MoveSingleFile(const char* from, const char* to, bool forceOverride)
            {
                if (!forceOverride && IsFileExists(to))
                {
                    return false;
                }

                return rename(from, to) == 0;
            }

            static void CopyDirectoryRecursively(const char* source, const char* destination)
            {
                CreateDirectories(destination, true);

                DIR* dir = opendir(source);
                if (!dir)
                {
                    return;
                }

                dirent* entry;
                while ((entry = readdir(dir)) != nullptr)
                {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    {
                        continue;
                    }

                    std::string srcPath(source);
                    srcPath += "/";
                    srcPath += entry->d_name;

                    std::string dstPath(destination);
                    dstPath += "/";
                    dstPath += entry->d_name;

                    if (entry->d_type == DT_DIR)
                    {
                        CopyDirectoryRecursively(srcPath.c_str(), dstPath.c_str());
                    }
                    else
                    {
                        CopySingleFile(srcPath.c_str(), dstPath.c_str(), true);
                    }
                }

                closedir(dir);
            }

            static void WalkThoughDirectory(const char* directory, std::function<bool(const char*, bool)> visitor, bool recursively, bool includeDirectories)
            {
                DIR* dir = opendir(directory);
                if (!dir)
                {
                    return;
                }

                dirent* entry;
                while ((entry = readdir(dir)) != nullptr)
                {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                    {
                        continue;
                    }

                    std::string fullPath(directory);
                    fullPath += "/";
                    fullPath += entry->d_name;

                    bool isDirectory = entry->d_type == DT_DIR;
                    if (isDirectory && recursively)
                    {
                        WalkThoughDirectory(fullPath.c_str(), visitor, true, includeDirectories);
                    }

                    if (isDirectory || includeDirectories)
                    {
                        if (!visitor(fullPath.c_str(), isDirectory))
                        {
                            break;
                        }
                    }
                }

                closedir(dir);
            }
        };
    }

    typedef PlatformPosix::FileSystemPosix FileSystemDetails;
}
#endif

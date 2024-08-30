#pragma once

#include <Common/Build.hpp>
#include <Common/DynamicBuffer.hpp>
#include <fstream>
#include <functional>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <FileSystem/Details/FileSystemWindows.hpp>
#else
#include <FileSystem/Details/FileSystemPosix.hpp>
#endif

namespace CppMiniToolkit
{
    class FileSystem
    {
    public:
        FileSystem() = delete;
        ~FileSystem() = delete;

        // is file or directory exists
        template <typename TCharType>
        static bool IsExists(const TCharType* path)
        {
            return FileSystemDetails::IsExists(path);
        }

        // is file exists
        template <typename TCharType>
        static bool IsFileExists(const TCharType* path)
        {
            return FileSystemDetails::IsFileExists(path);
        }

        // is directory exists
        template <typename TCharType>
        static bool IsDirectoryExists(const TCharType* directoryPath)
        {
            return FileSystemDetails::IsDirectoryExists(directoryPath);
        }

        // is directory empty
        template <typename TCharType>
        static bool IsDirectoryEmpty(const TCharType* directoryPath)
        {
            return FileSystemDetails::IsDirectoryEmpty(directoryPath);
        }

        // cretae directory if that directory not exists
        template <typename TCharType>
        static bool CreateDirectoryIfNotExists(const TCharType* directoryPath)
        {
            if(!IsDirectoryExists(directoryPath))
            {
                return CreateDirectories(directoryPath);
            }

            return true;
        }

        // create directory recursively
        template <typename TCharType>
        static bool CreateDirectories(const TCharType* directoryPath, bool recursively = true)
        {
            return FileSystemDetails::CreateDirectories(directoryPath, recursively);
        }

        // delete directory recursively
        template <typename TCharType>
        static bool DeleteDirectories(const TCharType* directoryPath, bool recursively = true)
        {
            return FileSystemDetails::DeleteDirectories(directoryPath, recursively);
        }

        // Delete one file
        template <typename TCharType>
        static bool DeleteSingleFile(const TCharType* path)
        {
            return FileSystemDetails::DeleteSingleFile(path);
        }

        // copy one file
        template <typename TCharType>
        static bool CopySingleFile(const TCharType* from, const TCharType* to, bool forceOverride = false)
        {
            return FileSystemDetails::CopySingleFile(from, to, forceOverride);
        }

        // move one file
        template <typename TCharType>
        static bool MoveSingleFile(const TCharType* from, const TCharType* to, bool forceOverride = false)
        {
            return FileSystemDetails::MoveSingleFile(from, to, forceOverride);
        }

        // copy a directory
        template <typename TCharType>
        static void CopyDirectoryRecursively(const TCharType* source, const TCharType* destination)
        {
            FileSystemDetails::CopyDirectoryRecursively(source, destination);
        }

        // walk though directory
        template <typename TCharType>
        static void WalkThoughDirectoryEx(const TCharType* directory, std::function<bool (const TCharType*,bool)> visitor, bool recursively = false, bool includeDirectories = true)
        {
            FileSystemDetails::WalkThoughDirectory(directory, visitor, recursively, includeDirectories);
        }

        // walk though without visitor
        template <typename TCharType>
        static void WalkThoughDirectory(const TCharType* directory, bool recursively = false, bool includeDirectories = true)
        {
            auto visitor = [](const TCharType* path, bool isDirectory) { return true; };
            WalkThoughDirectoryEx<TCharType>(
                directory,
                visitor,
                recursively,
                includeDirectories
                );
        }

        // get file length
        template <typename TCharType>
        static size_t GetFileLength(const TCharType* path)
        {
            std::ifstream file(path, std::ios::binary|std::ios::ate);
            if (!file)
            {
                return 0;
            }

            const std::streamsize size = file.tellg();
            return size;
        }

        template <typename TCharType>
        static bool ReadAllBytes(const TCharType* path, DynamicBuffer& buffer)
        {
            assert(path != nullptr);

            std::ifstream file(path, std::ios::binary|std::ios::ate);
            if (!file)
            {
                return false;
            }

            const std::streamsize size = file.tellg();
            file.seekg(0, std::ios::beg);

            buffer.Assign(size);
            if (file.read(reinterpret_cast<char*>(buffer.GetBuffer()), size))
            {
                return true;
            }

            return false;
        }

        template <typename TCharType>
        static DynamicBuffer ReadAllBytes(const TCharType* path)
        {
            DynamicBuffer buffer;
            ReadAllBytes(path, buffer);
            return buffer;
        }

        template <typename TCharType>
        static bool WriteAllBytes(const TCharType* path, const uint8_t* bytes, const size_t length)
        {
            assert(path != nullptr);

            std::ofstream file(path, std::ios::binary);
            if (!file)
            {
                return false;
            }

            file.write(reinterpret_cast<const char*>(bytes), static_cast<std::streamsize>(length));

            return static_cast<bool>(file);
        }

        template <typename TCharType>
        static bool WriteAllBytes(const TCharType* path, const DynamicBuffer& bytes)
        {
            return WriteAllBytes(path, bytes.GetData(), bytes.GetSize());
        }
    };
}

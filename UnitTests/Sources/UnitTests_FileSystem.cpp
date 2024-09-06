#include <gtest/gtest.h>
#include <Common/BuildConfig.hpp>
#include <FileSystem/FileSystem.hpp>
#include <FileSystem/Path.hpp>

using namespace CppMiniToolkit;

TEST(FileSystem, CreateAndDeleteDirectories)
{
    const char* dirPath = "testDir";
    ASSERT_TRUE(FileSystem::CreateDirectories(dirPath));
    ASSERT_TRUE(FileSystem::IsDirectoryExists(dirPath));
    ASSERT_TRUE(FileSystem::DeleteDirectories(dirPath));
    ASSERT_FALSE(FileSystem::IsDirectoryExists(dirPath));
}

TEST(FileSystem, WriteAndReadAllBytes)
{
    const char* filePath = "testFile.txt";
    const char* data = "Hello, World!";
    auto length = strlen(data) + 1;  // Include null-terminator

    ASSERT_TRUE(FileSystem::WriteAllBytes(filePath, reinterpret_cast<const uint8_t*>(data), length));

    DynamicBuffer buffer = FileSystem::ReadAllBytes(filePath);

    ASSERT_EQ(buffer.GetSize(), length);
    ASSERT_STREQ(reinterpret_cast<const char*>(buffer.GetBuffer()), data);

    ASSERT_TRUE(FileSystem::DeleteSingleFile(filePath));
}

TEST(FileSystem, WalkThoughDirectory)
{
    const char* dirPath = ".";
    FileSystem::WalkThoughDirectoryEx<char>(dirPath, [](const char* path, bool isDirectory)
    {
        std::cout << (isDirectory ? "Directory: " : "File: ") << path << std::endl;
        return true;  // Continue traversal
    }
    );
}


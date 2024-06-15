#include <gtest/gtest.h>
#include <Common/Build.hpp>
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

TEST(FileSystem, Path)
{
    EXPECT_EQ(PathUtils::GetFileName("C:\\1.txt"), "1.txt");
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension("C:\\1.txt"), "1");
    EXPECT_EQ(PathUtils::GetExtension("C:\\1.txt"), ".txt");
    EXPECT_EQ(PathUtils::ReplaceExtension("C:\\1.txt", ".exe"), "C:\\1.exe");

    EXPECT_EQ(PathUtils::GetDirectoryPath(L"C:\\1.txt"), L"C:\\");

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
    EXPECT_EQ(PathUtils::Combine("C:\\", "A", "BB", "CC.exe"), "C:\\A\\BB\\CC.exe");
    EXPECT_TRUE(PathUtils::IsAbsolutePath("C:\\AMD"));
#endif
    EXPECT_FALSE(PathUtils::IsAbsolutePath("../AMD"));

}

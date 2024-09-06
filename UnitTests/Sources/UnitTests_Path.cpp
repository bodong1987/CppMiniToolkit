#include <gtest/gtest.h>
#include <Common/Build.hpp>
#include <FileSystem/Path.hpp>

using namespace CppMiniToolkit;

TEST(PathUtils, GetFileName)
{
    EXPECT_EQ(PathUtils::GetFileName("C:\\folder\\file.txt"), "file.txt");
    EXPECT_EQ(PathUtils::GetFileName("/usr/local/bin/executable"), "executable");
    EXPECT_EQ(PathUtils::GetFileName("relative/path/to/file.txt"), "file.txt");
    EXPECT_EQ(PathUtils::GetFileName("file.txt"), "file.txt");
    EXPECT_EQ(PathUtils::GetFileName(""), "");
}

TEST(PathUtils, GetFileNameWithoutExtension)
{
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension("C:\\folder\\file.txt"), "file");
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension("/usr/local/bin/executable"), "executable");
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension("relative/path/to/file.txt"), "file");
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension("file.txt"), "file");
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension("file"), "file");
    EXPECT_EQ(PathUtils::GetFileNameWithoutExtension(""), "");
}

TEST(PathUtils, GetExtension)
{
    EXPECT_EQ(PathUtils::GetExtension("C:\\folder\\file.txt"), ".txt");
    EXPECT_EQ(PathUtils::GetExtension("/usr/local/bin/executable"), "");
    EXPECT_EQ(PathUtils::GetExtension("relative/path/to/file.txt"), ".txt");
    EXPECT_EQ(PathUtils::GetExtension("file.txt"), ".txt");
    EXPECT_EQ(PathUtils::GetExtension("file"), "");
    EXPECT_EQ(PathUtils::GetExtension(""), "");
}

TEST(PathUtils, ReplaceExtension)
{
    EXPECT_EQ(PathUtils::ReplaceExtension("C:\\folder\\file.txt", ".exe"), "C:\\folder\\file.exe");
    EXPECT_EQ(PathUtils::ReplaceExtension("/usr/local/bin/executable", ".bin"), "/usr/local/bin/executable.bin");
    EXPECT_EQ(PathUtils::ReplaceExtension("relative/path/to/file.txt", ".md"), "relative/path/to/file.md");
    EXPECT_EQ(PathUtils::ReplaceExtension("file.txt", ".cpp"), "file.cpp");
    EXPECT_EQ(PathUtils::ReplaceExtension("file", ".txt"), "file.txt");
    EXPECT_EQ(PathUtils::ReplaceExtension("", ".txt"), ".txt");
}

TEST(PathUtils, GetDirectoryPath)
{
    EXPECT_EQ(PathUtils::GetDirectoryPath("C:\\folder\\file.txt"), "C:\\folder");
    EXPECT_EQ(PathUtils::GetDirectoryPath("/usr/local/bin/executable"), "/usr/local/bin");
    EXPECT_EQ(PathUtils::GetDirectoryPath("relative/path/to/file.txt"), "relative/path/to");
    EXPECT_EQ(PathUtils::GetDirectoryPath("file.txt"), "");
    EXPECT_EQ(PathUtils::GetDirectoryPath(""), "");
}

TEST(PathUtils, GetDirectoryFileName)
{
    EXPECT_EQ(PathUtils::GetDirectoryFileName("C:\\folder\\file.txt"), "folder");
    EXPECT_EQ(PathUtils::GetDirectoryFileName("/usr/local/bin/executable"), "bin");
    EXPECT_EQ(PathUtils::GetDirectoryFileName("relative/path/to/file.txt"), "to");
    EXPECT_EQ(PathUtils::GetDirectoryFileName("file.txt"), "");
    EXPECT_EQ(PathUtils::GetDirectoryFileName(""), "");
}

TEST(PathUtils, IsExtension)
{
    EXPECT_TRUE(PathUtils::IsExtension("C:\\folder\\file.txt", ".txt"));
    EXPECT_FALSE(PathUtils::IsExtension("C:\\folder\\file.exe", ".txt"));
    EXPECT_FALSE(PathUtils::IsExtension("/usr/local/bin/executable", ".txt"));
    EXPECT_TRUE(PathUtils::IsExtension("file.txt", ".txt"));
    EXPECT_FALSE(PathUtils::IsExtension("file", ".txt"));
    EXPECT_FALSE(PathUtils::IsExtension("", ".txt"));
}

TEST(PathUtils, Combine)
{
#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
    EXPECT_EQ(PathUtils::Combine("C:\\", "folder", "file.txt"), "C:\\folder\\file.txt");
    EXPECT_EQ(PathUtils::Combine("C:\\folder", "subfolder", "file.txt"), "C:\\folder\\subfolder\\file.txt");
#else
    EXPECT_EQ(PathUtils::Combine("/usr", "local", "bin", "executable"), "/usr/local/bin/executable");
    EXPECT_EQ(PathUtils::Combine("relative", "path", "to", "file.txt"), "relative/path/to/file.txt");
#endif
    
    EXPECT_EQ(PathUtils::Combine("", "file.txt"), "file.txt");
}

TEST(PathUtils, IsAbsolutePath)
{
#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
    EXPECT_TRUE(PathUtils::IsAbsolutePath("C:\\folder\\file.txt"));
    EXPECT_TRUE(PathUtils::IsAbsolutePath("\\folder\\file.txt"));
#else
    EXPECT_TRUE(PathUtils::IsAbsolutePath("/usr/local/bin/executable"));
#endif
    
    EXPECT_FALSE(PathUtils::IsAbsolutePath("relative/path/to/file.txt"));
    EXPECT_FALSE(PathUtils::IsAbsolutePath("file.txt"));
    EXPECT_FALSE(PathUtils::IsAbsolutePath(""));
}

TEST(PathUtils, IsRelativePath)
{
#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
    EXPECT_FALSE(PathUtils::IsRelativePath("C:\\folder\\file.txt"));
    EXPECT_FALSE(PathUtils::IsRelativePath("\\folder\\file.txt"));
#else
    EXPECT_FALSE(PathUtils::IsRelativePath("/usr/local/bin/executable"));
#endif
    
    EXPECT_TRUE(PathUtils::IsRelativePath("relative/path/to/file.txt"));
    EXPECT_TRUE(PathUtils::IsRelativePath("file.txt"));
    EXPECT_TRUE(PathUtils::IsRelativePath(""));
}

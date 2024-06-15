#include <gtest/gtest.h>
#include <Common/Build.hpp>
#include <Encryption/MD5.hpp>
#include <Encryption/Crc32.hpp>
#include <Common/DynamicBuffer.hpp>
#include <FileSystem/FileSystem.hpp>

using namespace CppMiniToolkit;

TEST(MD5, CaculateBytes)
{
    const char* Text = "Hello World!";
    auto md5 = MD5::Caculate((const uint8_t*)Text, strlen(Text));
    std::string md5Text = md5.ToHexString();
    EXPECT_EQ(md5Text, "ed076287532e86365e841e92bfc50d8c");
}

TEST(MD5, CaculateFile)
{
    // Create a temporary file
    const char* tempFile = "temp.dat";

    DynamicBuffer buffer;
    // Write random data to the file    
    for (int i = 0; i < 50 * 1024 * 1024; ++i)
    {
        char c = static_cast<char>(i % 128 + 1);

        buffer.AppendValueBits(c);
    }

    FileSystem::WriteAllBytes(tempFile, buffer);

    auto md5 = MD5::CaculateFile(tempFile);
    auto md5Text = md5.ToHexUpperString();

    FileSystem::DeleteSingleFile(tempFile);

    ASSERT_EQ(md5Text, "785D79048A59072869DDEE449D559A95");
}


TEST(Crc32, CaculateBytes)
{
    const char* Text = "Hello World!";

    auto crc32 = CRC32::Caculate((const uint8_t*)Text, strlen(Text));    
    EXPECT_EQ(crc32, 0x1c291ca3);
}


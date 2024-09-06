#include <gtest/gtest.h>
#include <Text/Encoding.hpp>
#include <Text/Details/TextEncodingGeneric.hpp>

using namespace CppMiniToolkit;

TEST(Encoding, UTF16ToUTF8)
{
    std::u16string utf16 = u"Hello, World!";
    std::string utf8 = Encoding::UTF16ToUTF8(utf16.c_str());
    EXPECT_EQ(utf8, "Hello, World!");
}

TEST(Encoding, UTF8ToUTF16)
{
    std::string utf8 = "Hello, World!";
    std::u16string utf16 = Encoding::UTF8ToUTF16(utf8.c_str());
    EXPECT_EQ(utf16, u"Hello, World!");
}

TEST(Encoding, UTF16ToUTF8Chinese)
{
    std::u16string utf16 = u"\u4F60\u597D, \u4E16\u754C!";  // "你好, 世界!"
    std::string utf8 = Encoding::UTF16ToUTF8(utf16.c_str());
    EXPECT_EQ(utf8, "\xE4\xBD\xA0\xE5\xA5\xBD, \xE4\xB8\x96\xE7\x95\x8C!");  // "你好, 世界!" in UTF-8
}

TEST(Encoding, UTF8ToUTF16Chinese)
{
    std::string utf8 = "\xE4\xBD\xA0\xE5\xA5\xBD, \xE4\xB8\x96\xE7\x95\x8C!";  // "你好, 世界!" in UTF-8
    std::u16string utf16 = Encoding::UTF8ToUTF16(utf8.c_str());
    EXPECT_EQ(utf16, u"\u4F60\u597D, \u4E16\u754C!");  // "你好, 世界!"
}

#if CMT_PLATFORM_WINDOWS
TEST(Encoding, UTF16ToANSI)
{
    std::u16string utf16 = u"Hello, World!";
    std::string ansi = Encoding::UTF16ToANSI(utf16.c_str());
    EXPECT_EQ(ansi, "Hello, World!");
}

TEST(Encoding, ANSIToUTF16)
{
    std::string ansi = "Hello, World!";
    std::u16string utf16 = Encoding::ANSIToUTF16(ansi.c_str());
    EXPECT_EQ(utf16, u"Hello, World!");
}
#endif


TEST(EncodingGeneric, UTF16ToUTF8)
{
    std::u16string utf16 = u"Hello, World!";
    std::string utf8 = PlatformGeneric::TextEncodingGeneric::UTF16ToUTF8(utf16.c_str());
    EXPECT_EQ(utf8, "Hello, World!");
}

TEST(EncodingGeneric, UTF8ToUTF16)
{
    std::string utf8 = "Hello, World!";
    std::u16string utf16 = PlatformGeneric::TextEncodingGeneric::UTF8ToUTF16(utf8.c_str());
    EXPECT_EQ(utf16, u"Hello, World!");
}

TEST(EncodingGeneric, UTF16ToUTF8Chinese)
{
    std::u16string utf16 = u"\u4F60\u597D, \u4E16\u754C!";  // "你好, 世界!"
    std::string utf8 = PlatformGeneric::TextEncodingGeneric::UTF16ToUTF8(utf16.c_str());
    EXPECT_EQ(utf8, "\xE4\xBD\xA0\xE5\xA5\xBD, \xE4\xB8\x96\xE7\x95\x8C!");  // "你好, 世界!" in UTF-8
}

TEST(EncodingGeneric, UTF8ToUTF16Chinese)
{
    std::string utf8 = "\xE4\xBD\xA0\xE5\xA5\xBD, \xE4\xB8\x96\xE7\x95\x8C!";  // "你好, 世界!" in UTF-8
    std::u16string utf16 = PlatformGeneric::TextEncodingGeneric::UTF8ToUTF16(utf8.c_str());
    EXPECT_EQ(utf16, u"\u4F60\u597D, \u4E16\u754C!");  // "你好, 世界!"
}


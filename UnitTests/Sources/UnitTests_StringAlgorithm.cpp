#include <gtest/gtest.h>
#include <Algorithm/String.hpp>

using namespace CppMiniToolkit;

TEST(StringAlgorithm, CharTest)
{
    ASSERT_TRUE(StringAlgorithm::Equal("test", "test"));
    ASSERT_TRUE(StringAlgorithm::iEqual("test", "TEST"));
    ASSERT_TRUE(StringAlgorithm::StartWith("hello world", "hello"));
    ASSERT_TRUE(StringAlgorithm::iStartWith("hello world", "HELLO"));
    ASSERT_TRUE(StringAlgorithm::Contains("hello world", "world"));
    ASSERT_TRUE(StringAlgorithm::iContains("hello world", "WORLD"));
    ASSERT_TRUE(StringAlgorithm::EndWith("hello world", "world"));
    ASSERT_TRUE(StringAlgorithm::iEndWith("hello world", "WORLD"));

    std::string str = " hello world ";
    ASSERT_EQ(StringAlgorithm::Trim(str), "hello world");
    ASSERT_EQ(StringAlgorithm::ToUpper(str), "HELLO WORLD");
    ASSERT_EQ(StringAlgorithm::ToLower(str), "hello world");

    std::vector<std::string> vec = { "hello", "world" };
    ASSERT_EQ(StringAlgorithm::Join(vec, " "), "hello world");
}

TEST(StringAlgorithm, WCharTest)
{
    ASSERT_TRUE(StringAlgorithm::Equal(L"test", L"test"));
    ASSERT_TRUE(StringAlgorithm::iEqual(L"test", L"TEST"));
    ASSERT_TRUE(StringAlgorithm::StartWith(L"hello world", L"hello"));
    ASSERT_TRUE(StringAlgorithm::iStartWith(L"hello world", L"HELLO"));
    ASSERT_TRUE(StringAlgorithm::Contains(L"hello world", L"world"));
    ASSERT_TRUE(StringAlgorithm::iContains(L"hello world", L"WORLD"));
    ASSERT_TRUE(StringAlgorithm::EndWith(L"hello world", L"world"));
    ASSERT_TRUE(StringAlgorithm::iEndWith(L"hello world", L"WORLD"));

    std::wstring str = L" hello world ";
    ASSERT_EQ(StringAlgorithm::Trim(str), L"hello world");
    ASSERT_EQ(StringAlgorithm::ToUpper(str), L"HELLO WORLD");
    ASSERT_EQ(StringAlgorithm::ToLower(str), L"hello world");

    std::vector<std::wstring> vec = { L"hello", L"world" };
    ASSERT_EQ(StringAlgorithm::Join(vec, L" "), L"hello world");
}

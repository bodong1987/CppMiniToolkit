#include <gtest/gtest.h>
#include <Common/DynamicBuffer.hpp>
#include <Common/HasSignature.hpp>

using namespace CppMiniToolkit;

TEST(DynamicBuffer, DefaultConstructor) 
{
    DynamicBuffer buffer;
    EXPECT_EQ(buffer.GetSize(), 0);
    EXPECT_TRUE(buffer.IsEmpty());
}

TEST(DynamicBuffer, SizeConstructor)
{
    DynamicBuffer buffer(10);
    EXPECT_EQ(buffer.GetSize(), 10);
    EXPECT_FALSE(buffer.IsEmpty());
}

TEST(DynamicBuffer, CopyConstructor) 
{
    DynamicBuffer buffer1(10);
    DynamicBuffer buffer2(buffer1);
    EXPECT_EQ(buffer2.GetSize(), 10);
    EXPECT_FALSE(buffer2.IsEmpty());
}

TEST(DynamicBuffer, MoveConstructor) 
{
    DynamicBuffer buffer1(10);
    DynamicBuffer buffer2(std::move(buffer1));
    EXPECT_EQ(buffer2.GetSize(), 10);
    EXPECT_FALSE(buffer2.IsEmpty());
    EXPECT_EQ(buffer1.GetSize(), 0);
    EXPECT_TRUE(buffer1.IsEmpty());
}

TEST(DynamicBuffer, Append) 
{
    DynamicBuffer buffer;
    char data[10] = { 0 };
    buffer.Append(data, sizeof(data));
    EXPECT_EQ(buffer.GetSize(), 10);
    EXPECT_FALSE(buffer.IsEmpty());
}

TEST(DynamicBuffer, AssignSize)
{
    DynamicBuffer buffer;
    buffer.Assign(10);
    EXPECT_EQ(buffer.GetSize(), 10);
    EXPECT_FALSE(buffer.IsEmpty());
}

TEST(DynamicBuffer, AssignData)
{
    DynamicBuffer buffer;
    char data[10] = { 0 };
    buffer.Assign(data, sizeof(data));
    EXPECT_EQ(buffer.GetSize(), 10);
    EXPECT_FALSE(buffer.IsEmpty());
}

TEST(DynamicBuffer, Clear)
{
    DynamicBuffer buffer(10);
    buffer.Clear();
    EXPECT_EQ(buffer.GetSize(), 0);
    EXPECT_TRUE(buffer.IsEmpty());
}

TEST(DynamicBuffer, Shrink)
{
    DynamicBuffer buffer(10);
    buffer.Append("1234", 4);
    EXPECT_TRUE(buffer.GetCapacity() > buffer.GetSize());
    buffer.Shrink();
    EXPECT_EQ(buffer.GetSize(), 14);
    EXPECT_FALSE(buffer.IsEmpty());
}

TEST(DynamicBuffer, Reserve)
{
    DynamicBuffer buffer;
    buffer.Reserve(10);
    EXPECT_EQ(buffer.GetSize(), 0);
    EXPECT_TRUE(buffer.IsEmpty());
    buffer.Append("1234", 4);
    EXPECT_EQ(buffer.GetSize(), 4);
    EXPECT_FALSE(buffer.IsEmpty());
}

TEST(DynamicBuffer, AppendValueBits)
{
    DynamicBuffer buffer;
    int value = 1234;
    buffer.AppendValueBits(value);
    EXPECT_EQ(buffer.GetSize(), sizeof(value));
    EXPECT_FALSE(buffer.IsEmpty());
}

CPPMINITOOLKIT_DEFINE_HAS_SIGNATURE(HasStaticFooMemberFunction, T::foo, void (*)(void));

struct StaticFoo { static void foo(){} };
struct NormalFoo { void foo(){} };
struct NoFoo {};

static_assert(HasStaticFooMemberFunction<StaticFoo>::Value, "Unexpected value");
static_assert(!HasStaticFooMemberFunction<NormalFoo>::Value, "Unexpected value");
static_assert(!HasStaticFooMemberFunction<NoFoo>::Value, "Unexpected value");

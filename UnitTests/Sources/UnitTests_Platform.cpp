#include <gtest/gtest.h>

#include <Common/Build.hpp>

#include "FileSystem/Path.hpp"

#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
#include <Platform/Windows/Environment.hpp>
#include <Platform/Windows/ErrorHandling.hpp>
#include <Platform/Windows/ProcessMemory.hpp>
#include <Platform/Windows/ProcessPatchUtils.hpp>
#include <Platform/Windows/ThreadOperationUtils.hpp>
#include <Platform/Windows/ProcessOperationUtils.hpp>
#endif

using namespace CppMiniToolkit;


#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
TEST(WindowsPlatform, Environment)
{
    const auto systemVersion = Windows::Environment::GetSystemVersion();

    _tprintf_s(_T("Your System:%s\n"), systemVersion.c_str());
}

TEST(WindowsPlatform, Path)
{
    const auto appPath = Windows::Environment::GetEntryPath();
    const auto appDir = Windows::Environment::GetEntryDirectory();
    const auto MyName = PathUtils::GetFileName(appPath.c_str());
    
    _tprintf_s(_T("Your App:%s\n"), appPath.c_str());
    _tprintf_s(_T("Your App Directory:%s\n"), appDir.c_str());

    EXPECT_STREQ(MyName.c_str(), L"UnitTests.exe");
}

TEST(WindowsPlatform, QueryThreadInformation)
{
    Windows::ThreadOperationUtils::SetThreadName(GetCurrentThread(), L"Main-Thread");
    const auto threadName = Windows::ThreadOperationUtils::GetThreadName(GetCurrentThread());

    EXPECT_STREQ(threadName.c_str(), L"Main-Thread");
}

TEST(WindowsPlatform, QueryProcessInformation)
{
    const auto MyName = PathUtils::GetFileName(Windows::ProcessOperationUtils::GetProcessPath(GetCurrentProcess()).c_str());

    EXPECT_STREQ(MyName.c_str(), L"UnitTests.exe");
}
#endif


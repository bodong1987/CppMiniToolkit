#include <gtest/gtest.h>

#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <Platform/Windows/Environment.hpp>
#include <Platform/Windows/ErrorHandling.hpp>
#include <Platform/Windows/Path.hpp>
#include <Platform/Windows/ProcessMemory.hpp>
#endif

using namespace CppMiniToolkit;


#if CPPMINITOOLKIT_PLATFORM_WINDOWS
TEST(WindowsPlatform, Environment)
{
    auto systemVersion = Windows::Envrionment::GetSystemVersion();

    _tprintf_s(_T("Your System:%s\n"), systemVersion.c_str());
}

TEST(WindowsPlatform, Path)
{
    auto appPath = Windows::PathUtils::GetEntryPath();
    auto appDir = Windows::PathUtils::GetEntryDirectory();

    _tprintf_s(_T("Your App:%s\n"), appPath.c_str());
    _tprintf_s(_T("Your App Directory:%s\n"), appDir.c_str());
}
#endif


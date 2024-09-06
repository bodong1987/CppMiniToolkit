#pragma once
// ReSharper disable IdentifierTypo

#if (defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WINDOWS) || defined(_WIN64))
#define CPP_MINI_TOOLKIT_PLATFORM_WINDOWS   1
#else
#define CPP_MINI_TOOLKIT_PLATFORM_WINDOWS   0
#endif

// on apple platform ??? macos or ios ...
#if defined(__APPLE__) || defined(APPLE) || defined(_APPLE_) || defined(PLATFORM_APPLE)
#define CPP_MINI_TOOLKIT_PLATFORM_APPLE     1
#include <TargetConditionals.h>
#else
#define CPP_MINI_TOOLKIT_PLATFORM_APPLE     0  // NOLINT(modernize-macro-to-enum)
#endif

#if defined(__OBJC__)
#define CPP_MINI_TOOLKIT_OBJECTIVE_C        1  // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_OBJECTIVE_C        0  // NOLINT(modernize-macro-to-enum)
#endif

#ifdef _MSC_VER
#define CPP_MINI_TOOLKIT_COMPILER_MSVC      1
#else
#define CPP_MINI_TOOLKIT_COMPILER_MSVC      0
#endif

#ifdef __GNUC__
#define CPP_MINI_TOOLKIT_COMPILER_GCC       1
#else
#define CPP_MINI_TOOLKIT_COMPILER_GCC       0
#endif // __GNUC__

// ios or ios simulator
#if defined(TARGET_OS_IOS) || defined(TARGET_IPHONE_SIMULATOR)
#define CPP_MINI_TOOLKIT_PLATFORM_IOS       1   // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_PLATFORM_IOS       0   // NOLINT(modernize-macro-to-enum)
#endif

// iphone simulator ???
#if defined(TARGET_IPHONE_SIMULATOR)
#define CPP_MINI_TOOLKIT_PLATFORM_IPHONE_SIMULATOR 1  // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_PLATFORM_IPHONE_SIMULATOR 0  // NOLINT(modernize-macro-to-enum)
#endif

#if defined(__APPLE__) || defined(APPLE) || defined(_APPLE_)
#define CPP_MINI_TOOLKIT_PLATFORM_MACOS     1
#else
#define CPP_MINI_TOOLKIT_PLATFORM_MACOS     0
#endif

#if defined(ANDROID) || defined(__ANDROID__)
#define CPP_MINI_TOOLKIT_PLATFORM_ANDROID   1  // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_PLATFORM_ANDROID   0  // NOLINT(modernize-macro-to-enum)
#endif


#if defined(_WIN64) || defined(_X64) || defined(WIN64) || defined( __LP64__ )
#define CPP_MINI_TOOLKIT_PLATFORM_X64       1  // NOLINT(modernize-macro-to-enum)
#elif defined(WIN32) || defined(_WIN32)
#define CPP_MINI_TOOLKIT_PLATFORM_X64       0  // NOLINT(modernize-macro-to-enum)
#endif

#if defined(__arm__) || defined(__thumb__) || defined(_ARM) || defined(_M_ARM) || defined(_M_ARMT) || defined(__arm)
#define CPP_MINI_TOOLKIT_PLATFORM_ARM       1  // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_PLATFORM_ARM       0  // NOLINT(modernize-macro-to-enum)
#endif

#if defined(DEBUG)||defined(_DEBUG)
#define CPP_MINI_TOOLKIT_DEBUG              1  // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_DEBUG              0  // NOLINT(modernize-macro-to-enum)
#endif

#if defined(UNICODE)||defined(_UNICODE)
#define CPP_MINI_TOOLKIT_UNICODE            1  // NOLINT(modernize-macro-to-enum)
#else
#define CPP_MINI_TOOLKIT_UNICODE            0  // NOLINT(modernize-macro-to-enum)
#endif

// is C++/CLI enabled???
#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS && CPP_MINI_TOOLKIT_COMPILER_MSVC && (defined(_MANAGED)||defined(_M_CEE))
#define CPP_MINI_TOOLKIT_COMPILER_MSVC_WITH_CPP_CLI 1
#else
#define CPP_MINI_TOOLKIT_COMPILER_MSVC_WITH_CPP_CLI 0
#endif

// disable warning for C++/CLI
#if CPP_MINI_TOOLKIT_COMPILER_MSVC_WITH_CPP_CLI
#pragma warning(disable : 4793)
#endif

#if ( CPP_MINI_TOOLKIT_PLATFORM_WINDOWS && defined(_MSC_VER) && _MSC_VER>=1600 ) || \
    ( CPP_MINI_TOOLKIT_COMPILER_GCC && defined(__GXX_EXPERIMENTAL_CXX0X__) ) || \
    CPP_MINI_TOOLKIT_PLATFORM_MACOS 
#else
#error "This code requires a C++11 compliant compiler"
#endif

#ifndef _countof
#define _countof( Array ) (sizeof(Array)/sizeof(Array[0]))
#endif

#define CPP_MINI_TOOLKIT_UNREFERENCED_PARAMETER(p) (void)(p)
#define CPP_MINI_TOOLKIT_DECLARE_TOOLKIT_CLASS_TYPE(typeName) \
        typeName() = delete; \
        typeName(const typeName&) = delete; \
        typeName(typeName&&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
        typeName& operator=(const typeName&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
        typeName& operator=(typeName&&) = delete; /*NOLINT(bugprone-macro-parentheses)*/\
        ~typeName() = delete
#pragma once

#if !CPPMINITOOLKIT_PLATFORM_WINDOWS
#error "This header file is provide for windows only"
#endif

#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

namespace CppMiniToolkit
{
    namespace PlatformWindows
    {
        class TextEncodingWindows
        {
        public:
            TextEncodingWindows() = delete;
            ~TextEncodingWindows() = delete;

            static std::string UTF16ToUTF8(const char16_t* text)
            {
                const int nLen = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWSTR>(text), -1, nullptr, 0, nullptr, nullptr);

                if (nLen <= 0)
                {
                    return {};
                }

                const auto Dest = new char[nLen];
                
                WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWSTR>(text), -1, Dest, nLen, nullptr, nullptr);
                Dest[nLen - 1] = 0;

                std::string strTemp(Dest);
                delete[] Dest;

                return strTemp;
            }

            static std::u16string UTF8ToUTF16(const char* text)
            {
                const auto nChars = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);

                if (nChars <= 0)
                {
                    return {};
                }

                const auto Dest = new char16_t[nChars];

                MultiByteToWideChar(CP_UTF8, 0, text, -1, reinterpret_cast<LPWSTR>(Dest), nChars);

                std::u16string str(Dest);

                // delete it
                delete[] Dest;

                return str;
            }

            static std::wstring UTF8ToWCHAR(const char* text)
            {
                const auto nChars = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);

                if (nChars <= 0)
                {
                    return {};
                }

                const auto Dest = new wchar_t[nChars];

                MultiByteToWideChar(CP_UTF8, 0, text, -1, (LPWSTR)Dest, nChars);

                std::wstring str(Dest);

                // delete it
                delete[] Dest;

                return str;
            }

            static std::string UTF16ToANSI(const char16_t* text)
            {
                const auto nLen = WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<LPCWSTR>(text), -1, nullptr, 0, nullptr, nullptr);

                if (nLen <= 0)
                {
                    return {};
                }

                const auto Dest = new char[nLen];

                WideCharToMultiByte(CP_ACP, 0, reinterpret_cast<LPCWSTR>(text), -1, Dest, nLen, nullptr, nullptr);
                Dest[nLen - 1] = 0;

                std::string strTemp(Dest);
                delete[] Dest;

                return strTemp;
            }

            static std::u16string ANSIToUTF16(const char* text)
            {
                const auto nChars = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);

                if (nChars <= 0)
                {
                    return {};
                }

                const auto Dest = new char16_t[nChars];

                MultiByteToWideChar(CP_ACP, 0, text, -1, reinterpret_cast<LPWSTR>(Dest), nChars);

                std::u16string str(Dest);

                // delete it
                delete[] Dest;

                return str;
            }
        };
    }

    typedef PlatformWindows::TextEncodingWindows TextEncodingDetails;
}

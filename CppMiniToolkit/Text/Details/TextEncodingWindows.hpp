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
            TextEncodingWindows() = delete;
            ~TextEncodingWindows() = delete;
        public:
            static std::string UTF16ToUTF8(const char16_t* text)
            {
                int nLen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)text, -1, nullptr, 0, nullptr, nullptr);

                if (nLen <= 0)
                {
                    return std::string();
                }

                char* pszDst = new char[nLen];
                
                WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)text, -1, pszDst, nLen, nullptr, nullptr);
                pszDst[nLen - 1] = 0;

                std::string strTemp(pszDst);
                delete[] pszDst;

                return strTemp;
            }

            static std::u16string UTF8ToUTF16(const char* text)
            {
                int nChars = MultiByteToWideChar(CP_UTF8, 0, text, -1, nullptr, 0);

                if (nChars <= 0)
                {
                    return std::u16string();
                }

                char16_t* pwcsName = new char16_t[nChars];

                MultiByteToWideChar(CP_UTF8, 0, text, -1, (LPWSTR)pwcsName, nChars);

                std::u16string str(pwcsName);

                // delete it
                delete[] pwcsName;

                return str;
            }

            static std::string UTF16ToANSI(const char16_t* text)
            {
                int nLen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)text, -1, nullptr, 0, nullptr, nullptr);

                if (nLen <= 0)
                {
                    return std::string();
                }

                char* pszDst = new char[nLen];

                WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)text, -1, pszDst, nLen, nullptr, nullptr);
                pszDst[nLen - 1] = 0;

                std::string strTemp(pszDst);
                delete[] pszDst;

                return strTemp;
            }

            static std::u16string ANSIToUTF16(const char* text)
            {
                int nChars = MultiByteToWideChar(CP_ACP, 0, text, -1, nullptr, 0);

                if (nChars <= 0)
                {
                    return std::u16string();
                }

                char16_t* pwcsName = new char16_t[nChars];

                MultiByteToWideChar(CP_ACP, 0, text, -1, (LPWSTR)pwcsName, nChars);
                
                std::u16string str(pwcsName);

                // delete it
                delete[] pwcsName;

                return str;
            }
        };
    }

    typedef PlatformWindows::TextEncodingWindows TextEncodingDetails;
}

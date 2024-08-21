#pragma once

#include <string>

#include <Common/Build.hpp>

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
#include <Text/Details/TextEncodingWindows.hpp>
#else
#include <Text/Details/TextEncodingGeneric.hpp>
#endif

namespace CppMiniToolkit
{
    class Encoding
    {
    public:
        Encoding() = delete;
        ~Encoding() = delete;

        static std::string UTF16ToUTF8(const char16_t* text)
        {
            return TextEncodingDetails::UTF16ToUTF8(text);
        }

        static std::u16string UTF8ToUTF16(const char* text)
        {
            return TextEncodingDetails::UTF8ToUTF16(text);
        }

#if CPPMINITOOLKIT_PLATFORM_WINDOWS
        static std::string UTF16ToANSI(const char16_t* text)
        {
            return PlatformWindows::TextEncodingWindows::UTF16ToANSI(text);
        }

        static std::u16string ANSIToUTF16(const char* text)
        {
            return PlatformWindows::TextEncodingWindows::ANSIToUTF16(text);
        }

        static std::string WCHARToUTF8(const wchar_t* text)
        {
            return TextEncodingDetails::UTF16ToUTF8(reinterpret_cast<const char16_t*>(text));
        }

        static std::wstring UTF8ToWCHAR(const char* text)
        {
            return TextEncodingDetails::UTF8ToWCHAR(text);
        }
#endif

#if CPPMINITOOLKIT_COMPILER_MSVC_WITH_CPP_CLI        
        static System::String^ UTF8ToCLIString(const std::string& str)
        {
            if (str.empty())
            {
                return System::String::Empty;
            }

            array<System::Byte>^ bytes = gcnew array<System::Byte>((int)str.size());
            System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)str.c_str()), bytes, 0, (int)str.size());
            System::String^ text = System::Text::Encoding::UTF8->GetString(bytes);

            return text;
        }
                
        static System::String^ UTF16ToCLIString(const std::u16string& str)
        {
            if (str.empty())
            {
                return System::String::Empty;
            }

            array<System::Byte>^ bytes = gcnew array<System::Byte>((int)str.size() * sizeof(char16_t));
            System::Runtime::InteropServices::Marshal::Copy(System::IntPtr((void*)str.c_str()), bytes, 0, (int)str.size() * sizeof(char16_t));
            System::String^ text = System::Text::Encoding::Unicode->GetString(bytes);

            return text;
        }

        static std::u16string CLIStringToUTF16(System::String^ str)
        {
            if (str == nullptr)
            {
                return std::u16string();
            }

            const char16_t* chars = (const char16_t*)(System::Runtime::InteropServices::Marshal::StringToHGlobalUni(str).ToPointer());
            std::u16string s = chars;

            System::Runtime::InteropServices::Marshal::FreeHGlobal(System::IntPtr((void*)chars));

            return s;
        }

        static std::string CLIStringToUTF8(System::String^ str)
        {
            if (str == nullptr)
            {
                return std::string();
            }

            array<System::Byte>^ bytes = System::Text::Encoding::UTF8->GetBytes(str);
            pin_ptr<System::Byte> pinnedBytes = &bytes[0];
            std::string s((char*)pinnedBytes, bytes->Length);

            return s;
        }

#endif
    };
}

#define CPPMINITOOLKIT_UTF8_TO_UTF16(text) CppMiniToolkit::Encoding::UTF8ToUTF16(text)
#define CPPMINITOOLKIT_UTF16_TO_UTF8(text) CppMiniToolkit::Encoding::UTF8ToUTF16(text)

#pragma once

#include <Common/BuildConfig.hpp>
#include <string>
#include <Common/CharTraits.hpp>

namespace CppMiniToolkit
{
    class PathUtils
    {
    public:
        CPP_MINI_TOOLKIT_DECLARE_TOOLKIT_CLASS_TYPE(PathUtils);
        
    private:
        template <typename TCharType>
        static const TCharType* GetSplitFlags()
        {
            static const TCharType S_SplitFlags[] = { '\\', '/', 0 };
            return S_SplitFlags;
        }

    public:
        template <typename TCharType>
        static std::basic_string<TCharType> GetFileName(const TCharType* path)
        {
            auto pos = TCharTraits<TCharType>::rFindAny(path, GetSplitFlags<TCharType>());

            if (pos == nullptr)
            {
                return path;
            }

            return pos + 1;
        }

        template <typename TCharType>
        static std::basic_string<TCharType> GetExtension(const TCharType* path)
        {
            auto pos = TCharTraits<TCharType>::rFind(path, TCharType('.'));

            if (pos == nullptr)
            {
                return std::basic_string<TCharType>();
            }

            return pos;
        }

        template <typename TCharType>
        static std::basic_string<TCharType> GetFileNameWithoutExtension(const TCharType* path)
        {
            auto pos = TCharTraits<TCharType>::rFindAny(path, GetSplitFlags<TCharType>());
            auto startPos = pos == nullptr ? path : pos + 1;
            auto dotPos = TCharTraits<TCharType>::rFind(startPos, TCharType('.'));

            if (dotPos == nullptr)
            {
                return startPos;
            }

            return std::basic_string<TCharType>(startPos, dotPos);
        }

        template <typename TCharType>
        static bool IsExtension(const TCharType* path, const TCharType* extension)
        {
            const auto pathLength = std::char_traits<TCharType>::length(path);
            const auto extensionLength = std::char_traits<TCharType>::length(extension);

            return pathLength > extensionLength && 
                TCharTraits<TCharType>::iCompareN(path + pathLength - extensionLength, extension, extensionLength) == 0;
        }

        template <typename TCharType>
        static std::basic_string<TCharType> ReplaceExtension(const TCharType* path, const TCharType* extension)
        {
            auto pos = TCharTraits<TCharType>::rFind(path, TCharType('.'));

            std::basic_string<TCharType> Result = pos != nullptr? std::basic_string<TCharType>(path, pos) : path;

            Result += extension;

            return Result;
        }

        template <typename TCharType>
        static std::basic_string<TCharType> GetDirectoryPath(const TCharType* path)
        {
            auto len = std::char_traits<TCharType>::length(path);
            auto pos = TCharTraits<TCharType>::rFindAny(path, GetSplitFlags<TCharType>());

            return pos != nullptr ? std::basic_string<TCharType>(path, pos) : std::basic_string<TCharType>();
        }

        template <typename TCharType>
        static std::basic_string<TCharType> GetDirectoryFileName(const TCharType* path)
        {
            return GetFileName<TCharType>(GetDirectoryPath<TCharType>(path).c_str());
        }

    private:
        template <typename TCharType, typename T0, typename... T>
        struct CombineHelper
        {
            static std::basic_string<TCharType>& Combine(std::basic_string<TCharType>& path, const T0& arg0, const T... args)
            {
                if (!path.empty() && *path.rbegin() != '/' && *path.rbegin() != '\\')
                {
                    path.push_back(TCharTraits<TCharType>::StaticPathSeparator());
                }

                path += Shims::PtrOf(arg0);

                CombineHelper<TCharType, T...>::Combine(path, args...);

                return path;
            }
        };

        template <typename TCharType, typename T>
        struct CombineHelper<TCharType, T>
        {
            static std::basic_string<TCharType>& Combine(std::basic_string<TCharType>& path, const T& arg0)
            {
                if (!path.empty() && *path.rbegin() != '/' && *path.rbegin() != '\\')
                {
                    path.push_back(TCharTraits<TCharType>::StaticPathSeparator());
                }
                
                path += Shims::PtrOf(arg0);

                return path;
            }
        };
        
    public:
        template <typename TCharType, typename... T>
        static std::basic_string<TCharType> Combine(const TCharType* path, T... args)
        {
            std::basic_string<TCharType> Result = path;
            CombineHelper<TCharType, T...>::Combine(Result, args...);

            return Result;
        }

        template <typename TCharType>
        static bool IsAbsolutePath(const TCharType* path)
        {
#if CPP_MINI_TOOLKIT_PLATFORM_WINDOWS
            return (std::char_traits<TCharType>::length(path) >= 2 && path[1] == ':') || (path != nullptr && path[0] == '\\');
#else
            return path != nullptr && path[0] == '/';
#endif
        }

        template <typename TCharType>
        static bool IsRelativePath(const TCharType* path)
        {
            return !IsAbsolutePath<TCharType>(path);
        }
    };
}

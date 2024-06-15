#pragma once

#include <string>
#include <Common/CharTraits.hpp>

namespace CppMiniToolkit
{
    class StringAlgorithm
    {
    private:
        StringAlgorithm() = delete;
        ~StringAlgorithm() = delete;

        // equal
    public:
        template <typename TCharType>
        inline static bool Equal(const std::basic_string<TCharType>& first, const std::basic_string<TCharType>& second)
        {
            return first == second;
        }

        template <typename TCharType>
        inline static bool Equal(const std::basic_string<TCharType>& first, const TCharType* second)
        {
            return first == second;
        }

        template <typename TCharType>
        inline static bool Equal(const TCharType* first, const TCharType* second)
        {
            return TCharTraits<TCharType>::Compare(first, second) == 0;
        }

        // equal(ignore case)
        template <typename TCharType>
        inline static bool iEqual(const std::basic_string<TCharType>& first, const std::basic_string<TCharType>& second)
        {
            return TCharTraits<TCharType>::iCompare(first.c_str(), second.c_str()) == 0;
        }

        template <typename TCharType>
        inline static bool iEqual(const std::basic_string<TCharType>& first, const TCharType* second)
        {
            return TCharTraits<TCharType>::iCompare(first.c_str(), second) == 0;
        }

        template <typename TCharType>
        inline static bool iEqual(const TCharType* first, const TCharType* second)
        {
            return TCharTraits<TCharType>::iCompare(first, second) == 0;
        }

        // search and identifier
    public:
        template <typename TCharType>
        inline static bool StartWith(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& start)
        {
            return str.size() >= start.size() && TCharTraits<TCharType>::CompareN(str.c_str(), start.c_str(), start.size()) == 0;
        }

        template <typename TCharType>
        inline static bool StartWith(const std::basic_string<TCharType>& str, const TCharType* start)
        {
            std::size_t length = TCharTraits<TCharType>::length(start);

            return str.size() >= length && TCharTraits<TCharType>::CompareN(str.c_str(), start, length) == 0;
        }

        template <typename TCharType>
        inline static bool StartWith(const TCharType* str, const TCharType* start)
        {
            return TCharTraits<TCharType>::CompareN(str, start, TCharTraits<TCharType>::length(start)) == 0;
        }

        template <typename TCharType>
        inline static bool iStartWith(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& start)
        {
            return str.size() >= start.size() && TCharTraits<TCharType>::iCompareN(str.c_str(), start.c_str(), start.size()) == 0;
        }

        template <typename TCharType>
        inline static bool iStartWith(const std::basic_string<TCharType>& str, const TCharType* start)
        {
            std::size_t length = TCharTraits<TCharType>::length(start);

            return str.size() >= length && TCharTraits<TCharType>::iCompareN(str.c_str(), start, length) == 0;
        }

        template <typename TCharType>
        inline static bool iStartWith(const TCharType* str, const TCharType* start)
        {
            return TCharTraits<TCharType>::iCompareN(str, start, TCharTraits<TCharType>::length(start)) == 0;
        }

        template <typename TCharType>
        inline static bool Contains(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match)
        {
            return str.find(match) != std::basic_string<TCharType>::npos;
        }

        template <typename TCharType>
        inline static bool Contains(const std::basic_string<TCharType>& str, const TCharType* match)
        {
            return str.find(match) != std::basic_string<TCharType>::npos;
        }

        template <typename TCharType>
        inline static bool Contains(const TCharType* str, const TCharType* match)
        {
            return TCharTraits<TCharType>::Find(str, match) != nullptr;
        }

        template <typename TCharType>
        inline static bool iContains(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match)
        {
            return TCharTraits<TCharType>::iFind(str.c_str(), match.c_str()) != nullptr;
        }

        template <typename TCharType>
        inline static bool iContains(const std::basic_string<TCharType>& str, const TCharType* match)
        {
            return TCharTraits<TCharType>::iFind(str.c_str(), match) != nullptr;
        }
               
        template <typename TCharType>
        inline static bool iContains(const TCharType* str, const TCharType* match)
        {
            return TCharTraits<TCharType>::iFind(str, match) != nullptr;
        }

        template <typename TCharType>
        inline static bool EndWith(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match)
        {
            return str.size() >= match.size() && str.rfind(match) == str.size() - match.size();
        }

        template <typename TCharType>
        inline static bool EndWith(const std::basic_string<TCharType>& str, const TCharType* match)
        {
            std::size_t matchLength = TCharTraits<TCharType>::length(match);

            return str.size() >= matchLength && str.rfind(match) == str.size() - matchLength;
        }

        template <typename TCharType>
        inline static bool EndWith(const TCharType* str, const TCharType* match)
        {
            std::size_t strLength = TCharTraits<TCharType>::length(str);
            std::size_t matchLength = TCharTraits<TCharType>::length(match);

            return strLength >= matchLength && TCharTraits<TCharType>::CompareN(str + strLength - matchLength, match, matchLength) == 0;
        }

        template <typename TCharType>
        inline static bool iEndWith(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match)
        {
            const std::size_t strLength = str.size();
            const std::size_t matchLength = match.size();

            return strLength >= matchLength && TCharTraits<TCharType>::iCompareN(str.c_str() + strLength - matchLength, match.c_str(), matchLength) == 0;
        }

        template <typename TCharType>
        inline static bool iEndWith(const std::basic_string<TCharType>& str, const TCharType* match)
        {
            const std::size_t strLength = str.size();
            const std::size_t matchLength = TCharTraits<TCharType>::length(match);

            return strLength >= matchLength && TCharTraits<TCharType>::iCompareN(str.c_str() + strLength - matchLength, match, matchLength) == 0;
        }

        
        template <typename TCharType>
        inline static bool iEndWith(const TCharType* str, const TCharType* match)
        {
            const std::size_t strLength = TCharTraits<TCharType>::length(str);
            const std::size_t matchLength = TCharTraits<TCharType>::length(match);

            return strLength >= matchLength && TCharTraits<TCharType>::iCompareN(str + strLength - matchLength, match, matchLength) == 0;
        }

        // replace
    private:
        template <typename TCharType, bool ignoreCase>
        inline static std::basic_string<TCharType>& ReplaceCore(std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            auto ptr = ignoreCase ? TCharTraits<TCharType>::iFind(str.c_str(), match.c_str()) : TCharTraits<TCharType>::Find(str.c_str(), match.c_str());

            if (ptr != nullptr)
            {
                auto pos = ptr - str.c_str();

                str.replace(pos, match.size(), replace);
            }

            return str;
        }

    public:
        template <typename TCharType>
        inline static std::basic_string<TCharType>& Replace(std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            return ReplaceCore<TCharType, false>(str, match, replace);
        }
       
        template <typename TCharType>
        inline static std::basic_string<TCharType> ReplaceCopy(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            auto result = str;
            Replace(result, match, replace);
            return result;
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType>& iReplace(std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            return ReplaceCore<TCharType, true>(str, match, replace);
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType> iReplaceCopy(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            auto result = str;
            iReplace(result, match, replace);
            return result;
        }

    private:        
        template <typename TCharType, bool ignoreCase>
        inline static std::basic_string<TCharType>& ReplaceAllCore(std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            typename std::basic_string<TCharType>::size_type startPos = std::basic_string<TCharType>::npos;

            do
            {
                auto ptr = !ignoreCase ? TCharTraits<TCharType>::Find(str.c_str() + startPos, match.c_str()) : TCharTraits<TCharType>::iFind(str.c_str() + startPos, match.c_str());

                if (ptr == nullptr)
                {
                    break;
                }

                startPos = ptr - str.c_str() + match.size();
                str.replace(ptr - str.c_str(), match.size(), replace);
            } while (true);

            return str;
        }

    public:       
        template <typename TCharType>
        inline static std::basic_string<TCharType>& ReplaceAll(std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            return ReplaceAllCore<TCharType, false>(str, match, replace);
        }
                
        template <typename TCharType>
        inline static std::basic_string<TCharType> ReplaceAllCopy(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            auto result = str;
            ReplaceAll(result, match, replace);
            return result;
        }
            
        template <typename TCharType>
        inline static std::basic_string<TCharType>& iReplaceAll(std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            return ReplaceAllCore<TCharType, true>(str, match, replace);
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType> iReplaceAllCopy(const std::basic_string<TCharType>& str, const std::basic_string<TCharType>& match, const std::basic_string<TCharType>& replace)
        {
            auto result = str;
            iReplaceAll(result, match, replace);
            return result;
        }

        // trim
    public:        
        template <typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType>& TrimLeft(std::basic_string<TCharType>& str, Predicate predicate)
        {
            for (auto i = 0; i < str.size(); ++i)
            {
                if (!predicate(str[i]))
                {
                    if (i > 0)
                    {
                        str.erase(0, i);
                    }

                    break;
                }
            }

            return str;
        }

        template <typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType> TrimLeftCopy(const std::basic_string<TCharType>& str, Predicate predicate)
        {
            auto result = str;
            TrimLeft<TCharType, Predicate>(result, predicate);
            return result;
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType>& TrimLeft(std::basic_string<TCharType>& str)
        {
            return TrimLeft(str, TCharTraits<TCharType>::IsSpace);
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType> TrimLeftCopy(const std::basic_string<TCharType>& str)
        {
            auto result = str;
            TrimLeft<TCharType>(result);
            return result;
        }
       
        template <typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType>& TrimRight(std::basic_string<TCharType>& str, Predicate predicate)
        {
            if (str.empty())
            {
                return str;
            }

            for (auto i = str.size() - 1; i >= 0 && i < str.size(); --i)
            {
                if (!predicate(str[i]))
                {
                    if (i < str.size() - 1)
                    {
                        str.erase(i + 1, str.size() - i - 1);
                    }

                    break;
                }
            }

            return str;
        }

        template <typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType> TrimRightCopy(const std::basic_string<TCharType>& str, Predicate predicate)
        {
            auto result = str;
            TrimRight<TCharType, Predicate>(result, predicate);
            return result;
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType>& TrimRight(std::basic_string<TCharType>& str)
        {
            return TrimRight(str, TCharTraits<TCharType>::IsSpace);
        }
              
        template <typename TCharType>
        inline static std::basic_string<TCharType> TrimRightCopy(const std::basic_string<TCharType>& str)
        {
            auto result = str;
            TrimRight<TCharType>(result);
            return result;
        }

        template <typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType>& Trim(std::basic_string<TCharType>& str, Predicate predicate)
        {
            if (str.empty())
            {
                return str;
            }

            TrimLeft<TCharType, Predicate>(str, predicate);
            TrimRight<TCharType, Predicate>(str, predicate);
            return str;
        }
               
        template <typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType> TrimCopy(const std::basic_string<TCharType>& str, Predicate predicate)
        {
            auto result = str;
            Trim<TCharType, Predicate>(result, predicate);
            return result;
        }
       
        template <typename TCharType>
        inline static std::basic_string<TCharType>& Trim(std::basic_string<TCharType>& str)
        {
            TrimLeft<TCharType>(str);
            TrimRight<TCharType>(str);
            return str;
        }

        template <typename TCharType>
        inline static std::basic_string<TCharType> TrimCopy(const std::basic_string<TCharType>& str)
        {
            auto result = str;
            Trim<TCharType>(result);
            return result;
        }

        // upper/lower
    public:       
        template <typename TCharType>
        inline static std::basic_string<TCharType>& ToUpper(std::basic_string<TCharType>& str)
        {
            for (auto iter = str.begin(); iter != str.end(); ++iter)
            {
                *iter = TCharTraits<TCharType>::ToUpper(*iter);
            }

            return str;
        }
              
        template <typename TCharType>
        inline static std::basic_string<TCharType>& ToLower(std::basic_string<TCharType>& str)
        {
            for (auto iter = str.begin(); iter != str.end(); ++iter)
            {
                *iter = TCharTraits<TCharType>::ToLower(*iter);
            }

            return str;
        }
      
        template <typename TCharType>
        inline static std::basic_string<TCharType> ToUpperCopy(const std::basic_string<TCharType>& str)
        {
            auto result = str;
            ToUpper<TCharType>(result);
            return result;
        }
      
        template <typename TCharType>
        inline static std::basic_string<TCharType> ToLowerCopy(const std::basic_string<TCharType>& str)
        {
            auto result = str;
            ToLower<TCharType>(result);
            return result;
        }

        // find by predicate
    public:      
        template <typename TCharType, typename Predicate>
        inline static typename std::basic_string<TCharType>::size_type Find(const std::basic_string<TCharType>& str, Predicate predicate, typename std::basic_string<TCharType>::size_type startPos = 0)
        {
            for (auto i = startPos; i < str.size(); ++i)
            {
                if (predicate(str[i]))
                {
                    return i;
                }
            }

            return std::basic_string<TCharType>::npos;
        }

        // split
    public:     
        template <typename TSequenceType, typename TCharType, typename Predicate>
        inline static TSequenceType& Split(TSequenceType& sequence, const std::basic_string<TCharType>& str, Predicate predicate)
        {
            typedef typename std::basic_string<TCharType>::size_type size_type;

            size_type startPos = 0;
            size_type pos = std::basic_string<TCharType>::npos;
            while ((pos = Find<TCharType, Predicate>(str, predicate, startPos)) != std::basic_string<TCharType>::npos)
            {
                if (pos == startPos)
                {
                    startPos = pos + 1;
                    continue;
                }

                std::basic_string<TCharType> token = str.substr(startPos, pos - startPos);
                sequence.emplace_back(std::move(token));
                startPos = pos + 1;
            }

            return sequence;
        }

        // join
    public:     
        template <typename TSequenceType, typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType> Join(const TSequenceType& sequence, const TCharType* separator, Predicate predicate)
        {
            std::basic_string<TCharType> Result;

            auto itBegin = sequence.begin();
            auto itEnd = sequence.end();

            while (itBegin != itEnd && !predicate(*itBegin))
            {
                ++itBegin;
            }

            if (itBegin != itEnd)
            {
                Result.append(*itBegin);
                ++itBegin;
            }

            for (; itBegin != itEnd; ++itBegin)
            {
                if (predicate(*itBegin))
                {
                    Result.append(separator);
                    Result.append(*itBegin);
                }
            }

            return Result;
        }
              
        template <typename TSequenceType, typename TCharType, typename Predicate>
        inline static std::basic_string<TCharType> Join(const TSequenceType& sequence, const std::basic_string<TCharType>& separator, Predicate predicate)
        {
            return Join<TSequenceType, TCharType, Predicate>(sequence, separator.c_str(), predicate);
        }

        template <typename TSequenceType, typename TCharType>
        inline static std::basic_string<TCharType> Join(const TSequenceType& sequence, const TCharType* separator)
        {
            static auto predicate = [](const auto& x) { return true; };

            return Join<TSequenceType, TCharType, decltype(predicate)>(sequence, separator, predicate);
        }
       
        template <typename TSequenceType, typename TCharType>
        inline static std::basic_string<TCharType> Join(const TSequenceType& sequence, const std::basic_string<TCharType>& separator)
        {
            return Join<TSequenceType, TCharType>(sequence, separator.c_str());
        }
    };
}


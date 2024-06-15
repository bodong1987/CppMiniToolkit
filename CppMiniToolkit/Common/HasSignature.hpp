#pragma once

#include <cstdint>

#define CPPMINITOOLKIT_DEFINE_HAS_SIGNATURE(traitsName, funcName, signature)        \
    template <typename U>                                                           \
    class traitsName                                                                \
    {                                                                               \
    private:                                                                        \
        template<typename T, T> struct helper;                                      \
        template<typename T>                                                        \
        static std::uint8_t check(helper<signature, &funcName>*);                   \
        template<typename T> static std::uint16_t check(...);                       \
    public:                                                                         \
        static constexpr bool Value = sizeof(check<U>(0)) == sizeof(std::uint8_t);  \
    }



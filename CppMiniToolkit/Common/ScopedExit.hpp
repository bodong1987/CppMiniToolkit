#pragma once

namespace CppMiniToolkit
{
    namespace Details
    {
        template <typename TCallable>
        class ScopedExit 
        {
        private:
            // disable copy 
            ScopedExit(const ScopedExit&) = delete;
            ScopedExit& operator = (const ScopedExit&) = delete;
        public:
            explicit ScopedExit(TCallable func) noexcept :
                Func(std::move(func)), 
                engaged(true) 
            {
            }

            ScopedExit(ScopedExit&& other) noexcept : 
                Func(std::move(other.Func)), 
                engaged(other.engaged)
            {
                other.engaged = false;
            }

            ~ScopedExit()
            {
                if (engaged)
                {
                    Func();
                }
            }

        private:
            TCallable Func;
            bool engaged;
        };

        template <typename Function>
        inline ScopedExit<Function> MakeScopedExit(Function f) noexcept
        {
            return ScopedExit<Function>(std::move(f));
        }
    }

    // CAT
#define _CPPMINITOOLKIT_PP_CAT_IMPL_(a, b ) a ## b
#define CPPMINITOOLKIT_PP_CAT(a, b) _CPPMINITOOLKIT_PP_CAT_IMPL_( a, b )

#define CPPMINITOOLKIT_SCOPED_EXIT(expression) \
    auto CPPMINITOOLKIT_PP_CAT(scoped_exit_instance_, __COUNTER__) = ::CppMiniToolkit::Details::MakeScopedExit([&]() { expression; })
}

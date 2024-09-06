#pragma once

namespace CppMiniToolkit
{
    namespace Details
    {
        template <typename TCallable>
        class ScopedExit 
        {
        public:
            ScopedExit(const ScopedExit&) = delete;
            ScopedExit& operator = (const ScopedExit&) = delete;            
            ScopedExit& operator = (ScopedExit&&) = delete;

            explicit ScopedExit(TCallable func) noexcept :
                Func(std::move(func)), 
                Engaged(true) 
            {
            }

            ScopedExit(ScopedExit&& other) noexcept : 
                Func(std::move(other.Func)), 
                Engaged(other.Engaged)
            {
                other.Engaged = false;
            }

            ~ScopedExit()
            {
                if (Engaged)
                {
                    Func();
                }
            }

        private:
            TCallable Func;
            bool Engaged;
        };

        template <typename Function>
        ScopedExit<Function> MakeScopedExit(Function f) noexcept
        {
            return ScopedExit<Function>(std::move(f));
        }
    }

    // CAT
#define CPP_MINI_TOOLKIT_PP_CAT_IMPL(a, b ) a ## b
#define CPP_MINI_TOOLKIT_PP_CAT(a, b) CPP_MINI_TOOLKIT_PP_CAT_IMPL( a, b )

#define CPP_MINI_TOOLKIT_SCOPED_EXIT(expression) \
    auto CPP_MINI_TOOLKIT_PP_CAT(scoped_exit_instance_, __COUNTER__) = ::CppMiniToolkit::Details::MakeScopedExit([&]() { expression; })
}

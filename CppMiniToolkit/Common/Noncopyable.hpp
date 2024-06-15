#pragma once

namespace CppMiniToolkit
{
    class Noncopyable
    {
    public:
        Noncopyable() {}        
        ~Noncopyable() {}
    protected:
        Noncopyable(const Noncopyable&) = delete;
        Noncopyable& operator = (const Noncopyable&) = delete;
    };
}

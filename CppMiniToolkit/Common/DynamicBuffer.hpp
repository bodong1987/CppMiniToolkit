#pragma once

#include <cstdint>
#include <memory>
#include <cassert>
#include <type_traits>

namespace CppMiniToolkit
{
    template <int AlignLength = 4>
    class TDynamicBuffer
    {
    public:
        typedef std::allocator<uint8_t>			 AllocatorType;
        typedef size_t                           SizeType;

        explicit TDynamicBuffer()
        {
        }

        explicit TDynamicBuffer(const void* source, SizeType length)
        {
            Assign(source, length);
        }
                
        explicit TDynamicBuffer(SizeType size) 
        {
            Assign(size);
        }
                
        TDynamicBuffer(const TDynamicBuffer& other)
        {
            if (other.GetSize() > 0)
            {
                const SizeType asize = Align(other.GetSize());

                Buffer = Allocator.allocate(asize);

                assert(Buffer);

                memcpy(Buffer, other.GetData(), other.GetSize());

                Size = other.GetSize();
                AllocatedSize = asize;
            }
        }
                
        TDynamicBuffer(TDynamicBuffer&& other) noexcept 
        {
            Buffer = other.Buffer;
            Size = other.Size;
            AllocatedSize = other.AllocatedSize;

            other.Buffer = nullptr;
            other.Size = 0;
            other.AllocatedSize = 0;
        }

        TDynamicBuffer& operator = (const TDynamicBuffer& other)
        {
            if (this == &other)
            {
                return *this;
            }

            Clear();

            Assign(other.GetBuffer(), other.GetSize());

            return *this;
        }

        TDynamicBuffer& operator = (TDynamicBuffer&& other) noexcept
        {
            if (this == &other)
            {
                return *this;
            }

            Release();

            Buffer = other.Buffer;
            Size = other.Size;
            AllocatedSize = other.AllocatedSize;

            other.Buffer = nullptr;
            other.Size = 0;
            other.AllocatedSize = 0;

            return *this;
        }

        ~TDynamicBuffer()
        {
            Release();
        }

        void Clear()
        {
            Size = 0;
        }

        SizeType GetCapacity() const
        {
            return AllocatedSize;
        }

        SizeType GetSize() const
        {
            return Size;
        }

        bool IsEmpty() const
        {
            return GetSize() == 0;
        }

        uint8_t* GetBuffer()
        {
            return GetData();
        }

        const uint8_t* GetBuffer() const
        {
            return GetData();
        }

        uint8_t* GetData(SizeType off = 0)
        {
            if (!Buffer)
            {
                return nullptr;
            }

            assert(off < GetSize() && "invalid parameters!");

            return Buffer + off;
        }

        const uint8_t* GetData(SizeType off = 0) const
        {
            if (!Buffer)
            {
                return nullptr;
            }

            assert(off < GetSize() && "invalid parameters!");

            return Buffer + off;
        }
                
        uint8_t& At(SizeType index)
        {
            assert(index < GetSize() && "invalid parameters!");

            return Buffer[index];
        }

        const uint8_t& At(SizeType index) const
        {
            assert(index < GetSize() && "invalid parameters!");

            return Buffer[index];
        }

        uint8_t& operator [](SizeType index)
        {
            return At(index);
        }

        const uint8_t& operator [](SizeType index) const
        {
            return At(index);
        }

        template <typename T, typename _TEnableIfType = typename std::enable_if<std::is_pod<T>::value>::type>
        void AppendValueBits(T value)
        {
            Append(&value, sizeof(value));
        }

        void Append(const TDynamicBuffer& other, SizeType off = 0)
        {
            if (!other.IsEmpty() && other.GetSize() > off)
            {
                Append(other.GetData(off), other.GetSize() - off);
            }
        }
                
        void Append(const void* source, SizeType length)
        {
            assert(source && "input pSource can't be none!");

            if (length <= 0 || source == nullptr)
            {
                return;
            }

            if (AllocatedSize - Size >= length)
            {
                memcpy(Buffer + Size, source, length);
                Size += length;
            }
            else
            {
                auto current = Buffer;
                const SizeType old = Size;
                const SizeType aold = AllocatedSize;

                const SizeType asize = Align((AllocatedSize + length) * 2);

                Buffer = Allocator.allocate(asize);

                if (current && old > 0)
                {
                    memcpy(Buffer, current, old);
                }

                memcpy(Buffer + old, source, length);

                Size = old + length;
                AllocatedSize = asize;

                if (current)
                {
                    Allocator.deallocate(current, aold);
                }
            }
        }

        void Assign(SizeType size)
        {
            Clear();

            if (size > 0)
            {
                if (size <= AllocatedSize)
                {
                    Size = size;
                }
                else
                {
                    Release();

                    const SizeType asize = Align(size);

                    Buffer = Allocator.allocate(asize);
                    Size = size;
                    AllocatedSize = asize;
                }
            }
        }

        void Assign(const void* source, SizeType size)
        {
            Clear();

            Append(source, size);
        }
              
        bool Assign(SizeType offset, const void* source, SizeType size)
        {
            assert(offset + size <= GetSize());

            if (offset + size <= GetSize())
            {
                memcpy(&Buffer[offset], source, Size);
                return true;
            }

            return false;
        }

        void Shrink()
        {
            if (AllocatedSize > Align(Size))
            {
                if (Size > 0)
                {
                    const SizeType s = Size;
                    auto buf = Allocator.allocate(Align(s));

                    memcpy(buf, GetData(), Size);

                    Release();

                    Buffer = buf;
                    Size = s;
                    AllocatedSize = Align(s);
                }
                else
                {
                    Clear();
                }
            }
        }

        void Reserve(SizeType size)
        {
            if (size > AllocatedSize)
            {
                const SizeType s = Size;
                auto buf = Allocator.allocate(Align(size));

                if (Size > 0)
                {
                    memcpy(buf, GetData(), Size);
                }

                Release();

                AllocatedSize = Align(size);
                Size = s;
                Buffer = buf;
            }
        }
    private:
        void Release()
        {
            if (Buffer)
            {
                Allocator.deallocate(Buffer, AllocatedSize);
                Buffer = nullptr;
            }

            Size = AllocatedSize = 0;
        }

    private:
        static SizeType Align(SizeType size)
        {
            return (size + AlignLength - 1) & (~(AlignLength - 1));
        }

    private:
        uint8_t*                Buffer = nullptr;
        SizeType				Size = 0;
        SizeType				AllocatedSize = 0;
        AllocatorType			Allocator;
    };

    typedef TDynamicBuffer<>    DynamicBuffer;
}

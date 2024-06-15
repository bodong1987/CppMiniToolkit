#pragma once

#include <cstdint>
#include <memory>
#include <cassert>
#include <string>
#include <array>
#include <fstream>

namespace CppMiniToolkit
{
    struct MD5Value
    {
        union 
        {
            uint8_t Bytes[16];
        };

        MD5Value()
        {
            memset(Bytes, 0, sizeof(Bytes));
        }

        // Convert to a string of hexadecimal digits
        std::string ToHexString() const
        {
            char buffer[36];
            for (int i = 0; i < 16; ++i)
            {
                sprintf(buffer + i * 2, "%02x", Bytes[i]);
            }

            return buffer;
        }

        std::string ToHexUpperString() const
        {
            char buffer[36];
            for (int i = 0; i < 16; ++i)
            {
                sprintf(buffer + i * 2, "%02X", Bytes[i]);
            }

            return buffer;
        }
    };

    // @reference : https://github.com/dnomd343/md5sum
    class MD5
    {
    private:
        /// MD5 fixed constants in little endian.
        static const uint32_t kA = 0x67452301;
        static const uint32_t kB = 0xefcdab89;
        static const uint32_t kC = 0x98badcfe;
        static const uint32_t kD = 0x10325476;

        struct MD5Context
        {
            uint32_t A = kA;
            uint32_t B = kB;
            uint32_t C = kC;
            uint32_t D = kD;
            uint64_t size = 0; 
        };

    public:
        MD5& Reset()
        {
            Context = MD5Context();
            BufferSize = 0;

            return *this;
        }

        MD5& Update(const void* bytes, size_t length)
        {
            if (BufferSize != 0) 
            {
                if (BufferSize + length < 64)
                { 
                    // buffer not filled
                    memcpy(Buffer + BufferSize, bytes, length);

                    BufferSize += length;

                    return *this; // save into buffer and return
                }

                const auto size = 64 - BufferSize;
                memcpy(Buffer + BufferSize, bytes, size);

                UpdateCore(Buffer, 64); // fill and update with buffer

                bytes = static_cast<const char*>(bytes) + size;
                BufferSize = 0;
                length -= size;
            } // buffer is empty for now

            bytes = UpdateCore(bytes, length);
            length &= 0b111111; // length -> [0, 64)
            if (length != 0)
            {
                memcpy(Buffer, bytes, length); // save remain bytes into buffer
                BufferSize = length;
            }
            return *this;
        }

        MD5& Finalize()
        {
            FinalizeCore(Buffer, BufferSize);
            return *this;
        }

        MD5Value GetDigest() const
        {
            return *(MD5Value*)&Context;
        }

    private:
        /// MD5 data block index, input between 0 and 63.
        constexpr int K(const int i) 
        {
            constexpr int step[4] = { 1, 5, 3, 7 };
            constexpr int begin[4] = { 0, 1, 5, 0 };
            return (begin[i >> 4] + step[i >> 4] * i) & 0b1111;
        }

        /// MD5 circular shift times, input between 0 and 63.
        constexpr int S(const int i) 
        {
            constexpr int shift[4][4] = 
            {
                {7, 12, 17, 22},
                {5, 9, 14, 20},
                {4, 11, 16, 23},
                {6, 10, 15, 21},
            };
            return shift[i >> 4][i & 0b11];
        }

        constexpr static const uint32_t kT[64] = 
        {
               0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee ,
               0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501 ,
               0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be ,
               0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821 ,
               0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa ,
               0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8 ,
               0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed ,
               0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a ,
               0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c ,
               0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70 ,
               0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05 ,
               0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665 ,
               0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039 ,
               0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1 ,
               0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1 ,
               0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 
        };

        /// MD5 T-table constant, input between 0 and 63.
        constexpr uint32_t T(const int i) 
        {
            return kT[i];
        }
        
#define __MD5_R1 A, B, C, D
#define __MD5_R2 D, A, B, C
#define __MD5_R3 C, D, A, B
#define __MD5_R4 B, C, D, A

#define __MD5_F(x, y, z) (z ^ (x & (y ^ z)))
#define __MD5_G(x, y, z) (y ^ (z & (x ^ y)))
#define __MD5_H(x, y, z) (x ^ y ^ z)
#define __MD5_I(x, y, z) (y ^ (x | ~z))

#define __MD5_ROUND(i, f, a, b, c, d)           \
    do {                                      \
        a += f(b, c, d) + block[K(i)] + T(i); \
        a = a << S(i) | a >> (32 - S(i));     \
        a += b;                               \
    } while (0)

#ifdef _MSC_VER
#define __MD5_EXPAND(...) __VA_ARGS__
#define __MD5_ROUND_WRAPPER(...) __MD5_EXPAND(__MD5_ROUND(__VA_ARGS__))
#else
#define __MD5_ROUND_WRAPPER __MD5_ROUND
#endif

#define __MD5_FF(i, ...) __MD5_ROUND_WRAPPER(i | 0x00, __MD5_F, __VA_ARGS__)
#define __MD5_GG(i, ...) __MD5_ROUND_WRAPPER(i | 0x10, __MD5_G, __VA_ARGS__)
#define __MD5_HH(i, ...) __MD5_ROUND_WRAPPER(i | 0x20, __MD5_H, __VA_ARGS__)
#define __MD5_II(i, ...) __MD5_ROUND_WRAPPER(i | 0x30, __MD5_I, __VA_ARGS__)

#define __MD5_UPDATE(OP)                                  \
    OP(0x0, __MD5_R1); OP(0x1, __MD5_R2); OP(0x2, __MD5_R3); OP(0x3, __MD5_R4); \
    OP(0x4, __MD5_R1); OP(0x5, __MD5_R2); OP(0x6, __MD5_R3); OP(0x7, __MD5_R4); \
    OP(0x8, __MD5_R1); OP(0x9, __MD5_R2); OP(0xa, __MD5_R3); OP(0xb, __MD5_R4); \
    OP(0xc, __MD5_R1); OP(0xd, __MD5_R2); OP(0xe, __MD5_R3); OP(0xf, __MD5_R4);

        const void* UpdateCore(const void* bytes, size_t length)
        {
            const uint32_t* block = static_cast<const uint32_t*>(bytes);
            const void* limit = block + ((length &= ~0b111111ULL) >> 2);

            auto A = Context.A;
            auto B = Context.B;
            auto C = Context.C;
            auto D = Context.D;

            while (block < limit)
            {
                const auto A_ = A;
                const auto B_ = B;
                const auto C_ = C;
                const auto D_ = D;
                __MD5_UPDATE(__MD5_FF)
                __MD5_UPDATE(__MD5_GG)
                __MD5_UPDATE(__MD5_HH)
                __MD5_UPDATE(__MD5_II)
                
                A += A_;
                B += B_;
                C += C_;
                D += D_;
                block += 16; // move to next block
            }

            Context.A = A;
            Context.B = B;
            Context.C = C;
            Context.D = D;
            Context.size += length;

            return limit;
        }
#undef __MD5_UPDATE
#undef __MD5_FF
#undef __MD5_GG
#undef __MD5_HH
#undef __MD5_II
#undef __MD5_ROUND
#ifdef _MSC_VER
#undef __MD5_EXPAND
#endif
#undef __MD5_ROUND_WRAPPER
#undef __MD5_F
#undef __MD5_G
#undef __MD5_H
#undef __MD5_I
#undef __MD5_R1
#undef __MD5_R2
#undef __MD5_R3
#undef __MD5_R4

        MD5& FinalizeCore(const void* bytes, size_t length)
        {
            static constexpr unsigned char Padding[64]{ 0x80, /* 0x00, ... */ };

            if (length >= 120)
            { 
                // length -> [64 + 56, INF)
                bytes = UpdateCore(bytes, length);
                length &= 0b111111; // length -> [0, 64)
            }

            unsigned char buffer[128]; // 2 blocks
            memcpy(buffer, bytes, length);
            const uint64_t total = (Context.size + length) << 3; // total number in bit

            if (length < 56) 
            { 
                // length -> [0, 56)
                memcpy(buffer + length, Padding, 56 - length);
                memcpy(buffer + 56, &total, 8);
                UpdateCore(buffer, 64); // update 1 block
            }
            else 
            { 
                // length -> [56, 64 + 56)
                memcpy(buffer + length, Padding, 120 - length);
                memcpy(buffer + 120, &total, 8);
                UpdateCore(buffer, 128); // update 2 blocks
            }

            return *this;
        }


    public:
        static MD5Value Caculate(const uint8_t* bytes, size_t length)
        {
            MD5 md5;
            md5.FinalizeCore(bytes, length);
            return md5.GetDigest();
        }

        template <typename TCharType>
        static MD5Value CaculateFile(const TCharType* path)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file) 
            {
                return MD5Value();
            }

            MD5 md5;
            uint8_t buffer[4096];

            while (!file.eof()) 
            {
                file.read(reinterpret_cast<char*>(buffer), sizeof(buffer));

                auto size = file.gcount();

                if (size > 0) 
                {
                    md5.Update(buffer, size);
                }
            }

            md5.Finalize();

            return md5.GetDigest();
        }

    private:
        MD5Context Context;
        uint8_t    Buffer[64] = {0};
        uint64_t   BufferSize = 0;
    };
}

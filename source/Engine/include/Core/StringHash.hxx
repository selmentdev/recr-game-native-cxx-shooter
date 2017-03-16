#ifndef INCLUDED_CORE_STRINGHASH_HXX
#define INCLUDED_CORE_STRINGHASH_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core
{
    namespace FNV1A64
    {
        using hash_t = std::uint64_t;

        constexpr const hash_t Prime = UINT64_C(0x100000001B3);
        constexpr const hash_t Basis = UINT64_C(0xCBF29CE484222325);

        constexpr hash_t CompileTime(const char* string, hash_t last_value = Basis) noexcept
        {
            return (*string) ? CompileTime(string + 1, ((hash_t)*string ^ last_value) * Prime) : last_value;
        }

        inline hash_t RunTime(const char* string) noexcept
        {
            hash_t result{ Basis };
            while (*string)
            {
                result ^= (hash_t)*string;
                result *= Prime;
                ++string;
            }

            return result;
        }
    }

    constexpr FNV1A64::hash_t operator ""_hash64(const char* p, size_t)
    {
        return FNV1A64::CompileTime(p);
    }

    using hash64_t = FNV1A64::hash_t;
}


namespace Core
{
    namespace FNV1A32
    {
        using hash_t = std::uint32_t;

        constexpr const hash_t Prime = UINT32_C(0x01000193);
        constexpr const hash_t Basis = UINT32_C(0x811C9DC5);

        constexpr hash_t CompileTime(const char* string, hash_t last_value = Basis) noexcept
        {
            //
            // Meh, we DO want to overflow it. Besides, unsigned integer overflow is well defined anyway.
            //
            // Possible MSVS bug.
            //
            return (*string) ? CompileTime(string + 1, static_cast<hash_t>((hash_t)*string ^ last_value) * static_cast<uint64_t>(Prime)) : last_value;
        }

        inline hash_t RunTime(const char* string) noexcept
        {
            hash_t result{ Basis };
            while (*string)
            {
                result ^= (hash_t)*string;
                result *= Prime;
                ++string;
            }

            return result;
        }
    }

    using hash32_t = FNV1A32::hash_t;

    constexpr FNV1A32::hash_t operator ""_hash32(const char* p, size_t)
    {
        return FNV1A32::CompileTime(p);
    }

}

#endif // INCLUDED_CORE_STRINGHASH_HXX
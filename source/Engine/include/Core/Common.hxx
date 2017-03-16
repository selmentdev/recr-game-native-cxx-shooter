#ifndef INCLUDE_CORE_COMMON_HXX
#define INCLUDE_CORE_COMMON_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <array>
#include <chrono>
#include <cinttypes>
#include <codecvt>
#include <cstdarg>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <wrl/client.h>

//
// Implements bit ops for specified enum class.
//
#define CORE_ENUM_CLASS_FLAGS(_EnumType) \
    inline _EnumType& operator |= (_EnumType& lvalue, const _EnumType rvalue) noexcept { return lvalue = static_cast<_EnumType>(static_cast<std::underlying_type_t<_EnumType>>(lvalue) | static_cast<std::underlying_type_t<_EnumType>>(rvalue)); } \
    inline _EnumType& operator &= (_EnumType& lvalue, const _EnumType rvalue) noexcept { return lvalue = static_cast<_EnumType>(static_cast<std::underlying_type_t<_EnumType>>(lvalue) & static_cast<std::underlying_type_t<_EnumType>>(rvalue)); } \
    inline _EnumType& operator ^= (_EnumType& lvalue, const _EnumType rvalue) noexcept { return lvalue = static_cast<_EnumType>(static_cast<std::underlying_type_t<_EnumType>>(lvalue) & static_cast<std::underlying_type_t<_EnumType>>(rvalue)); } \
    inline constexpr _EnumType operator | (const _EnumType lvalue, const _EnumType rvalue) noexcept { return static_cast<_EnumType>(static_cast<std::underlying_type_t<_EnumType>>(lvalue) | static_cast<std::underlying_type_t<_EnumType>>(rvalue)); } \
    inline constexpr _EnumType operator & (const _EnumType lvalue, const _EnumType rvalue) noexcept { return static_cast<_EnumType>(static_cast<std::underlying_type_t<_EnumType>>(lvalue) & static_cast<std::underlying_type_t<_EnumType>>(rvalue)); } \
    inline constexpr _EnumType operator ^ (const _EnumType lvalue, const _EnumType rvalue) noexcept { return static_cast<_EnumType>(static_cast<std::underlying_type_t<_EnumType>>(lvalue) ^ static_cast<std::underlying_type_t<_EnumType>>(rvalue)); } \
    inline constexpr bool operator ! (const _EnumType value) noexcept { return !static_cast<std::underlying_type_t<_EnumType>>(value); } \
    inline constexpr _EnumType operator ~ (const _EnumType value) noexcept { return static_cast<_EnumType>(~static_cast<std::underlying_type_t<_EnumType>>(value)); }


namespace Core
{
    template <typename T>
    __forceinline T Clamp(T value, T minValue, T maxValue) noexcept
    {
        return (std::min)((std::max)(minValue, value), maxValue);
    }
}

#endif // INCLUDE_CORE_COMMON_HXX
#ifndef INCLUDED_CORE_STRINGFORMAT_HXX
#define INCLUDED_CORE_STRINGFORMAT_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core
{
    std::string StringFormatArgs(const char* format, va_list arglist) noexcept;
    std::string StringFormat(const char* format, ...) noexcept;
}

#endif // INCLUDED_CORE_STRINGFORMAT_HXX
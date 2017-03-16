//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/StringFormat.hxx>

namespace Core
{

    std::string StringFormatArgs(const char* format, va_list arglist) noexcept
    {
        //
        // Create output string. This uses NRVO.
        //
        std::string result{};

        //
        // Compute number of characters required to format string.
        //
        va_list args_to_check;
        va_copy(args_to_check, arglist);
#if defined(_MSC_VER)
        const int count = ::_vscprintf(format, args_to_check);
#else
        char tmpbuf[1];
        const int count = ::vsnprintf(tmpbuf, 0, format, args_to_check);
#endif
        va_end(args_to_check);

        //
        // Check whether string format is possible
        //
        if (count >= 0)
        {
            //
            // Allocate space in string.
            //
            result.resize(static_cast<size_t>(count));

            //
            // Do actual string format.
            //
            va_list args_to_format;
            va_copy(args_to_format, arglist);
#if defined(_MSC_VER)
            const int processed = ::_vsnprintf_s(
                &result[0],
                result.length() + 1,
                result.length(),
                format,
                args_to_format
            );
#else
            const int processed = ::vsnprintf(
                &result[0],
                result.length() + 1,
                format,
                args_to_format
            );
#endif
            va_end(args_to_format);

            //
            // Check if format was successful.
            //
            if (processed < 0)
            {
                result.clear();
            }
        }

        return result;
    }

    std::string StringFormat(const char* format, ...) noexcept
    {
        //
        // Create output string. This uses NRVO.
        //
        std::string result{};

        //
        // Compute number of characters required to format string.
        //
        va_list args_to_check;
        va_start(args_to_check, format);
#if defined(_MSC_VER)
        const int count = ::_vscprintf(format, args_to_check);
#else
        char tmpbuf[1];
        const int count = ::vsnprintf(tmpbuf, 0, format, args_to_check);
#endif
        va_end(args_to_check);

        //
        // Check whether string format is possible
        //
        if (count >= 0)
        {
            //
            // Allocate space in string.
            //
            result.resize(static_cast<size_t>(count));

            //
            // Do actual string format.
            //
            va_list args_to_format;
            va_start(args_to_format, format);
#if defined(_MSC_VER)
            const int processed = ::_vsnprintf_s(
                &result[0],
                result.length() + 1,
                result.length(),
                format,
                args_to_format
            );
#else
            const int processed = ::vsnprintf(
                &result[0],
                result.length() + 1,
                format,
                args_to_format
            );
#endif
            va_end(args_to_format);

            //
            // Check if format was successful.
            //
            if (processed < 0)
            {
                result.clear();
            }
        }

        return result;
    }
}
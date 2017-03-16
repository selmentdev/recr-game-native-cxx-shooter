#ifndef INCLUDED_CORE_DIAGNOSTICS_TRACE_HXX
#define INCLUDED_CORE_DIAGNOSTICS_TRACE_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core::Diagnostics
{
    enum class TraceLevel
    {
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
    };

    class Trace final
    {
    private:
        static TraceLevel m_CurrentLevel;

    public:
        Trace() = delete;
        Trace(const Trace&) = delete;
        Trace& operator = (const Trace&) = delete;

    public:
        static void Initialize() noexcept;
        static void Shutdown() noexcept;

    public:
        static void WriteLine(const char* format, ...) noexcept;

    public:
        static inline bool CanDispatch(TraceLevel level) noexcept
        {
            using T = std::underlying_type_t<TraceLevel>;

            return static_cast<T>(level) >= static_cast<T>(Trace::m_CurrentLevel);
        }
    };

#define CORE_TRACE_MESSAGE(_Level, _Format, ...) \
    do \
    { \
        if (::Core::Diagnostics::Trace::CanDispatch(::Core::Diagnostics::TraceLevel::_Level)) \
        { \
            ::Core::Diagnostics::Trace::WriteLine(_Format, ## __VA_ARGS__); \
        } \
    } while (false)
}

#endif // INCLUDED_CORE_DIAGNOSTICS_TRACE_HXX
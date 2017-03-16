#ifndef INCLUDED_CORE_DIAGNOSTICS_DEBUG_HXX
#define INCLUDED_CORE_DIAGNOSTICS_DEBUG_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core::Diagnostics
{
    class Debug final
    {
    public:
        Debug() = delete;
        Debug(const Debug&) = delete;
        Debug& operator = (const Debug&) = delete;

    public:
        static void Initialize() noexcept;
        static void Shutdown() noexcept;

    public:
        static void WriteLine(const char* line) noexcept;
        static void Fail(const char* message) noexcept;
        static bool AssertionFailed(const char* message, const char* function, const char* file, unsigned int line) noexcept;
    };

#ifdef NDEBUG

#define CORE_ASSERT_MSG(_Expression, _Message)      ((void)0)
#define CORE_ASSERT(_Expression)                    ((void)0)

#else

#define CORE_ASSERT_MSG(_Expression, _Message) \
    do \
    { \
        if (!(_Expression)) \
        { \
            if (::Core::Diagnostics::Debug::AssertionFailed(_Message, __FUNCTION__, __FILE__, __LINE__) == false) \
            { \
                ::DebugBreak(); \
            } \
        } \
    } while(false)

#define CORE_ASSERT(_Expression) \
    do \
    { \
        if (!(_Expression)) \
        { \
            if (::Core::Diagnostics::Debug::AssertionFailed(nullptr, __FUNCTION__, __FILE__, __LINE__) == false) \
            { \
                ::DebugBreak(); \
            } \
        } \
    } while(false)

#endif

}

#endif // INCLUDED_CORE_DIAGNOSTICS_DEBUG_HXX
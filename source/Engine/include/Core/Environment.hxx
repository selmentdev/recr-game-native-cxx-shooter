#ifndef INCLUDED_CORE_ENVIRONMENT_HXX
#define INCLUDED_CORE_ENVIRONMENT_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core
{
    class Environment final
    {
    public:
        Environment() = delete;
        Environment(const Environment&) = delete;
        Environment& operator = (const Environment&) = delete;

    private:
        static void* s_InstanceHandle;
        static bool s_IsExitRequested;

    public:
        static inline void* InstanceHandle() noexcept
        {
            return s_InstanceHandle;
        }
        static inline bool IsExitRequested() noexcept
        {
            return s_IsExitRequested;
        }

    public:
        static void Initialize(void* instanceHandle) noexcept;
        static void Shutdown() noexcept;

    public:
        static std::string GetBasePath() noexcept;

    public:
        static void RequestExit() noexcept;
    };
}


#endif // INCLUDED_CORE_ENVIRONMENT_HXX
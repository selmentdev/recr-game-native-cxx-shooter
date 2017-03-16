#ifndef INCLUDED_CORE_TIMER_HXX
#define INCLUDED_CORE_TIMER_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

namespace Core
{
    class Timer final
    {
    private:
        double m_Scale;
        double m_DeltaTime;

    private:
        int64_t m_BaseTime;
        int64_t m_PausedTime;
        int64_t m_StopTime;
        int64_t m_PreviousTime;
        int64_t m_CurrentTime;

        bool m_IsStopped;

    public:
        Timer() noexcept;

    public:
        float TotalTime() const noexcept;
        float DeltaTime() const noexcept;

    public:
        void Reset() noexcept;
        void Start() noexcept;
        void Stop() noexcept;
        void Tick() noexcept;
    };
}

#endif // INCLUDED_CORE_TIMER_HXX
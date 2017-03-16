//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Timer.hxx>
#include <Windows.h>
#include <Core/StringFormat.hxx>

namespace Core
{
    Timer::Timer() noexcept
        : m_Scale{ 0.0 }
        , m_DeltaTime{ 0.0 }
        , m_BaseTime{}
        , m_PausedTime{}
        , m_StopTime{}
        , m_PreviousTime{}
        , m_CurrentTime{}
    {
        ::LARGE_INTEGER liFrequency{};
        ::QueryPerformanceFrequency(&liFrequency);

        m_Scale = 1.0 / static_cast<double>(liFrequency.QuadPart);
    }

    float Timer::TotalTime() const noexcept
    {
        //
        // Compute total time based on actual timer state.
        //
        if (m_IsStopped)
        {
            return static_cast<float>(
                ((m_StopTime - m_PausedTime) - m_BaseTime) * m_Scale
                );
        }
        else
        {
            return static_cast<float>(
                ((m_CurrentTime - m_PausedTime) - m_BaseTime) * m_Scale
                );
        }
    }

    float Timer::DeltaTime() const noexcept
    {
        return static_cast<float>(m_DeltaTime);
    }

    void Timer::Reset() noexcept
    {
        ::LARGE_INTEGER liCurrentTime{};
        ::QueryPerformanceCounter(&liCurrentTime);

        m_BaseTime = liCurrentTime.QuadPart;
        m_PreviousTime = liCurrentTime.QuadPart;
        m_StopTime = 0;
        m_IsStopped = false;
    }

    void Timer::Start() noexcept
    {
        ::LARGE_INTEGER liCurrentTime{};
        ::QueryPerformanceCounter(&liCurrentTime);

        if (m_IsStopped)
        {
            m_PausedTime += (liCurrentTime.QuadPart - m_StopTime);
            m_PreviousTime = liCurrentTime.QuadPart;
            m_StopTime = 0;
            m_IsStopped = false;
        }
    }

    void Timer::Stop() noexcept
    {
        if (!m_IsStopped)
        {
            ::LARGE_INTEGER liCurrentTime{};
            ::QueryPerformanceCounter(&liCurrentTime);

            m_StopTime = liCurrentTime.QuadPart;
            m_IsStopped = true;
        }
    }

    void Timer::Tick() noexcept
    {
        if (m_IsStopped)
        {
            m_DeltaTime = 0.0;
        }
        else
        {
            ::LARGE_INTEGER liCurrentTime{};
            ::QueryPerformanceCounter(&liCurrentTime);

            m_CurrentTime = liCurrentTime.QuadPart;
            m_DeltaTime = (m_CurrentTime - m_PreviousTime) * m_Scale;
            m_PreviousTime = m_CurrentTime;

            m_DeltaTime = (std::max)(m_DeltaTime, 0.0);
        }
    }
}

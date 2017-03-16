//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Diagnostics/Trace.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <fstream>
#include <array>
#include <cstdarg>
#include <chrono>
#include <iomanip>
#include <ctime>

namespace Core::Diagnostics
{
    namespace
    {
        constexpr const size_t TraceMessageSize = 256;
        std::ofstream TraceOutputLog;
    }

    TraceLevel Trace::m_CurrentLevel = TraceLevel::Debug;

    void Trace::Initialize() noexcept
    {
        //
        // Initialize debug log :)
        //
        TraceOutputLog.open("debug.log", std::ios::trunc | std::ios::binary | std::ios::out);
    }

    void Trace::Shutdown() noexcept
    {
        TraceOutputLog.flush();
    }

    void Trace::WriteLine(const char* format, ...) noexcept
    {
        std::array<char, TraceMessageSize> buffer{};

        va_list arglist;
        va_start(arglist, format);

        //
        // Format message.
        //
        auto result = std::vsnprintf(buffer.data(), buffer.size(), format, arglist);
        if (result < 0)
        {
            Debug::Fail("Trace::WriteLine failed");
        }

        //
        // Output to debug.
        //
        Debug::WriteLine(buffer.data());

        //
        // Get current time as ISO 8601
        //
        {
            auto t = std::time(nullptr);
            std::tm time{};

            localtime_s(&time, &t);

            TraceOutputLog << std::put_time(&time, "[%d-%m-%Y %H-%M-%S] ");
        }

        //
        // And to trace log
        //
        TraceOutputLog << buffer.data() << std::endl;

        va_end(arglist);
    }
}
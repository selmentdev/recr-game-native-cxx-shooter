//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Environment.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <Core.Diagnostics/Trace.hxx>
#include <codecvt>

namespace Core
{
    void* Environment::s_InstanceHandle = nullptr;
    bool Environment::s_IsExitRequested = false;

    void Environment::Initialize(void* instanceHandle) noexcept
    {
        //
        // Initialize environment.
        //
        Environment::s_InstanceHandle = instanceHandle;
        Diagnostics::Debug::Initialize();
        Diagnostics::Trace::Initialize();

        CORE_TRACE_MESSAGE(Info, "Welcome to `Core Prototype Engine`!");
        CORE_TRACE_MESSAGE(Debug, "Current directory: `%s`", Environment::GetBasePath().c_str());
    }

    void Environment::Shutdown() noexcept
    {
        //
        // Shutdown environment.
        //

        CORE_TRACE_MESSAGE(Info, "Shutting down `Core Prototype Engine`. Bye!");

        Diagnostics::Trace::Shutdown();
        Diagnostics::Debug::Shutdown();
    }

    std::string Environment::GetBasePath() noexcept
    {
        std::wstring result{};

        //
        // Get working directory.
        //
        ::DWORD dwLength = ::GetCurrentDirectoryW(0, nullptr);
        result.resize(static_cast<size_t>(dwLength));
        ::GetCurrentDirectoryW(dwLength, &result[0]);

        //
        // Alloc converter.
        //
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};

        //
        // And convert.
        //
        return converter.to_bytes(result);
    }

    void Environment::RequestExit() noexcept
    {
        //
        // Exit request will just break main loop.
        //
        CORE_TRACE_MESSAGE(Info, "Requested engine exit.");
        Environment::s_IsExitRequested = true;
    }
}

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Diagnostics/Debug.hxx>
#include <Core.Diagnostics/Trace.hxx>
#include <Core/StringFormat.hxx>
#include <Core/Environment.hxx>
#include <Windows.h>
#include <codecvt>
#include <sstream>

#include <DbgHelp.h>
#include <crtdbg.h>

#pragma comment(lib, "Dbghelp.lib")

namespace Core::Diagnostics
{
    //
    // Whoosh, a lot of things :)
    //
    namespace
    {
        //
        // Just various Windows validation / exception handling / C++ standard library / CRT library functionalities.
        //
        static ::LPTOP_LEVEL_EXCEPTION_FILTER debug_unhandled_exception_filter = nullptr;
        static ::_purecall_handler debug_purecall_handler = nullptr;
        static ::_invalid_parameter_handler debug_invalid_parameter_handler = nullptr;
        static std::terminate_handler debug_terminate_handler = nullptr;
        static std::unexpected_handler debug_unexpected_handler = nullptr;
        static ::PHANDLER_ROUTINE debug_control_c_handler = nullptr;

        void __cdecl error_handling_terminate_handler() noexcept
        {
            Debug::Fail("Terminate Handler");
        }

        void __cdecl error_handling_unexpected_handler() noexcept
        {
            Debug::Fail("Unexpected Handler");
        }

        void __cdecl error_handling_purecall_handler() noexcept
        {
            Debug::Fail("Pure function call handler");
        }

        void __cdecl error_handling_invalid_parameter_handler(
            const wchar_t* expression,
            const wchar_t* function,
            const wchar_t* file,
            unsigned int line,
            uintptr_t reserved
        ) noexcept
        {
            (void)reserved;
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter{};

            std::string n_expression = converter.to_bytes(expression);
            std::string n_function = converter.to_bytes(function);
            std::string n_file = converter.to_bytes(file);

            std::string message = StringFormat(
                "Invalid parameter detected in CRT function\n"
                "\n"
                "Expresion: \"%s\"\n"
                "Function: %s\n"
                "File: %s\n"
                "Line: %u\n",
                n_expression.c_str(),
                n_function.c_str(),
                n_file.c_str(),
                line
            );

            Debug::Fail(message.c_str());
        }

        ::BOOL CALLBACK error_handling_minidump_handler(::PVOID param, const ::PMINIDUMP_CALLBACK_INPUT input, ::PMINIDUMP_CALLBACK_OUTPUT output) noexcept
        {
            (void)param;

            ::BOOL result = FALSE;

            if ((input == nullptr) || (output == nullptr))
            {
                return FALSE;
            }

            switch (input->CallbackType)
            {
            case ::IncludeModuleCallback:
            case ::IncludeThreadCallback:
            case ::ThreadCallback:
            case ::ThreadExCallback:
            case ::MemoryCallback:
                result = TRUE;
                break;

            case ::ModuleCallback:
                {
                    if ((output->ModuleWriteFlags & ::ModuleReferencedByMemory) != 0)
                    {
                        output->ModuleWriteFlags ^= ::ModuleWriteModule;
                    }

                    result = TRUE;
                    break;
                }

            case ::CancelCallback:
                break;
            }

            return result;
        }

        ::LONG CALLBACK error_handling_unhandled_exception_filter(::EXCEPTION_POINTERS* ep) noexcept
        {
            if (ep != nullptr)
            {
                std::string filename{ "minidump.dmp" };

                //
                // Open handle to minidump file.
                //
                ::HANDLE file = ::CreateFileA(
                    filename.c_str(),
                    GENERIC_WRITE,
                    0,
                    nullptr,
                    CREATE_ALWAYS,
                    FILE_ATTRIBUTE_NORMAL,
                    nullptr
                );

                if (file != INVALID_HANDLE_VALUE)
                {
                    ::MINIDUMP_EXCEPTION_INFORMATION mdei;
                    mdei.ThreadId = ::GetCurrentThreadId();
                    mdei.ExceptionPointers = ep;
                    mdei.ClientPointers = FALSE;

                    ::MINIDUMP_CALLBACK_INFORMATION mci;
                    mci.CallbackParam = nullptr;
                    mci.CallbackRoutine = static_cast<::MINIDUMP_CALLBACK_ROUTINE>(error_handling_minidump_handler);

                    ::MINIDUMP_TYPE mdt = static_cast<::MINIDUMP_TYPE>(
                        ::MiniDumpWithPrivateWriteCopyMemory |
                        ::MiniDumpScanMemory);

                    ::MiniDumpWriteDump(
                        ::GetCurrentProcess(),
                        ::GetCurrentProcessId(),
                        file,
                        mdt,
                        (ep != nullptr) ? &mdei : nullptr,
                        0,
                        &mci);

                    ::CloseHandle(file);

                    auto message = StringFormat(
                        "Minidump saved to file: \"%s\"\n"
                        "Exception catched: 0x%08X at 0x%p\n",
                        filename.c_str(),
                        ep->ExceptionRecord->ExceptionCode,
                        ep->ExceptionRecord->ExceptionAddress
                    );

                    {
                        for (::DWORD i = 0; i < std::min<::DWORD>(ep->ExceptionRecord->NumberParameters, EXCEPTION_MAXIMUM_PARAMETERS); ++i)
                        {
                            message += StringFormat("Param%d: %p\n", i, ep->ExceptionRecord->ExceptionInformation[i]);
                        }
                    }

                    Debug::Fail(message.c_str());
                }
            }

            return EXCEPTION_EXECUTE_HANDLER;
        }

        ::BOOL WINAPI error_handling_handler_routine_handler(::DWORD ctrl_type) noexcept
        {
            (void)ctrl_type;


            if (Environment::IsExitRequested())
            {
                ::PostQuitMessage(0);
                Environment::RequestExit();
            }
            else
            {
                Debug::Fail("");
            }

            return TRUE;
        }
    }

    void Debug::Initialize() noexcept
    {
        //
        // Real engine may initialize any system handlers, i.e:
        //
        // - exception handlers (SEH, VEH),
        // - application termination handlers (std::set_terminate, std::set_unexpected),
        // - CRT library validation layer handler,
        // - purecall handler
        // - Ctrl+C handler
        // - creating minidumps on crash.
        //
#if defined(_DEBUG) && defined(_MSC_VER)
        //
        // On windows, enable CRT debugging facilities.
        //
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
        //
        // Register unhandled exception filter.
        //
        debug_unhandled_exception_filter = ::SetUnhandledExceptionFilter(error_handling_unhandled_exception_filter);

        //
        // Register terminate handler.
        //
        debug_terminate_handler = std::set_terminate(error_handling_terminate_handler);

        //
        // Register unexpected handler.
        //
        debug_unexpected_handler = std::set_unexpected(error_handling_unexpected_handler);

        //
        // Register invalid parameter handler.
        //
        debug_invalid_parameter_handler = _set_invalid_parameter_handler(error_handling_invalid_parameter_handler);

        //
        // Register purecall handler.
        //
        debug_purecall_handler = _set_purecall_handler(error_handling_purecall_handler);

        ::SetConsoleCtrlHandler(error_handling_handler_routine_handler, TRUE);
    }

    void Debug::Shutdown() noexcept
    {
    }

    void Debug::WriteLine(const char* line) noexcept
    {
        ::OutputDebugStringA(line);
        ::OutputDebugStringA("\n");
    }

    void Debug::Fail(const char* message) noexcept
    {
        //
        // Output message line to debug output.
        //
        Trace::WriteLine("%s", message);

        //
        // Convert message to 
        //
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};
        auto wmessage = converter.from_bytes(message);

        //
        // Show message to user.
        //
        ::MessageBoxW(
            0,
            wmessage.c_str(),
            L"Failure",
            MB_OK
        );

        //
        // Shutdown diagnostics.
        //
        Debug::Shutdown();
        Trace::Shutdown();

        //
        // And terminate process.
        //
        ::TerminateProcess(::GetCurrentProcess(), EXIT_FAILURE);
    }

    bool Debug::AssertionFailed(const char* message, const char* function, const char* file, unsigned int line) noexcept
    {
        //
        // Format message.
        //
        std::stringstream ss{};
        ss << "Assertion failed:\n";

        if (message != nullptr)
        {
            ss << " - message: `" << message << "`\n";
        }

        ss << " - location: `" << function << " at " << file << ':' << line << '\n';

        //
        // Write line to log.
        //
        auto text = ss.str();
        Trace::WriteLine("%s", text.c_str());

        //
        // And show message box.
        //
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};
        auto wtext = converter.from_bytes(text);

        switch (::MessageBoxW(0, wtext.c_str(), L"Assertion failed", MB_OK | MB_ABORTRETRYIGNORE))
        {
        case IDRETRY:
            {
                return false;
            }
        case IDIGNORE:
            {
                return true;
            }
        case IDABORT:
            {
                Debug::Fail("Abort due to assertion failure");
                return false;
            }
        }

        //
        // Default failsafe.
        //
        return false;
    }
}
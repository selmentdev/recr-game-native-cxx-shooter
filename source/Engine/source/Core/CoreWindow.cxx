//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/CoreWindow.hxx>
#include <Core/CoreApplication.hxx>
#include <Core/Environment.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <codecvt>

namespace Core
{
    CoreWindow::CoreWindow(const CoreWindowDesc& desc) noexcept
        : m_Desc{ desc }
        , m_Handle{}
    {
        //
        // Window Style.
        //
        ::DWORD dwStyle
            = WS_POPUP
            | WS_OVERLAPPED
            | WS_SYSMENU
            | WS_BORDER
            | WS_CAPTION;

        //
        // And extended style.
        //
        ::DWORD dwExStyle
            = WS_EX_APPWINDOW;

        //
        // Setup client rect.
        //
        ::RECT rcClient{ 0, 0, desc.Width, desc.Height };

        //
        // And adjust it to style.
        //
        ::AdjustWindowRectEx(&rcClient, dwStyle, FALSE, dwExStyle);

        //
        // Create window.
        //
        auto windowWidth = rcClient.right - rcClient.left;
        auto windowHeight = rcClient.bottom - rcClient.top;

        auto handle = ::CreateWindowExW(
            dwExStyle,
            L"CoreWindow",
            L"CoreWindow",
            dwStyle,
            100,
            100,
            windowWidth,
            windowHeight,
            nullptr,
            nullptr,
            reinterpret_cast<::HINSTANCE>(Environment::InstanceHandle),
            reinterpret_cast<::LPVOID>(this)
        );

        CORE_ASSERT(handle != nullptr);
        m_Handle = handle;

        //
        // And set additional data for handle.
        //
        ::SetWindowLongPtrW(
            m_Handle,
            GWLP_USERDATA,
            reinterpret_cast<::LONG_PTR>(this)
        );
    }

    CoreWindow::~CoreWindow() noexcept
    {
        //
        // Destroy window.
        //
        if (m_Handle == nullptr)
        {
            ::DestroyWindow(m_Handle);
        }
    }

    void CoreWindow::Show() noexcept
    {
        //
        // Show & update.
        //
        ::ShowWindow(m_Handle, SW_SHOW);
        ::UpdateWindow(m_Handle);
    }

    void CoreWindow::Hide() noexcept
    {
        //
        // Hide window.
        //
        ::ShowWindow(m_Handle, SW_HIDE);
    }

    void CoreWindow::SetFullscreen(bool value) noexcept
    {
        //
        // Not implemented. Sorry. Use Alt+Enter on Direct3D 11.
        //
        (void)value;
    }

    void CoreWindow::SetEnabled(bool value) noexcept
    {
        //
        // Well, enable / disable window.
        //
        ::EnableWindow(m_Handle, value ? TRUE : FALSE);
    }

    void CoreWindow::SetText(const char* text) noexcept
    {
        //
        // Set window text.
        //

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};

        ::SetWindowTextW(m_Handle, converter.from_bytes(text).c_str());
    }

    void CoreWindow::Destroy() noexcept
    {
        //
        // Destroy window manually.
        //
        ::DestroyWindow(m_Handle);

        m_Handle = nullptr;
    }

    void CoreWindow::TryRegisterWindowClass(::HINSTANCE instance, ::HICON icon) noexcept
    {
        //
        // Register own class.
        //
        ::WNDCLASSEXW wcex{};
        wcex.cbSize = sizeof(wcex);
        wcex.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = CoreApplication::ApplicationWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = instance;
        wcex.hIcon = icon;
        wcex.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<::HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"CoreWindow";
        wcex.hIconSm = icon;

        auto result = ::RegisterClassExW(&wcex);

        CORE_ASSERT(result != 0);
        (void)result;
    }
}

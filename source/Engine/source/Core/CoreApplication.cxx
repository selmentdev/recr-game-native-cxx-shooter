//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/CoreApplication.hxx>
#include <Core/Environment.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <vector>

namespace Core
{
    //
    // Borrowed from Graphyte Engine.
    //

    CoreApplication* CoreApplication::Current{ nullptr };

    CoreApplication::CoreApplication() noexcept
    {
        //
        // Make this current application.
        //
        CORE_ASSERT(CoreApplication::Current == nullptr);

        CoreApplication::Current = this;

        //
        // Register window class at startup.
        //
        CoreWindow::TryRegisterWindowClass(
            reinterpret_cast<::HINSTANCE>(Environment::InstanceHandle),
            ::LoadIconW(nullptr, IDI_APPLICATION)
        );
    }

    CoreApplication::~CoreApplication() noexcept
    {
        //
        // Cleanup current application.
        //
        CORE_ASSERT(CoreApplication::Current != nullptr);
        CoreApplication::Current = nullptr;
    }

    CoreWindowRef CoreApplication::MakeWindow(const CoreWindowDesc& desc) noexcept
    {
        return MakeRef<CoreWindow>(desc);
    }

    CoreApplicationRef CoreApplication::MakeApplication() noexcept
    {
        return MakeRef<CoreApplication>();
    }

    void CoreApplication::Tick(float deltaTime) noexcept
    {
        (void)deltaTime;
    }

    void CoreApplication::PumpMessages() noexcept
    {
        //
        // Pump messages until queue empty.
        //
        ::MSG message{};

        while (::PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE) != FALSE)
        {
            ::TranslateMessage(&message);
            ::DispatchMessageW(&message);
        }
    }

    ::LRESULT CoreApplication::ProcessMessage(::HWND handle, ::UINT message, ::WPARAM wparam, ::LPARAM lparam) noexcept
    {
        //
        // You're never sure.
        //
        static_assert(sizeof(::LONG_PTR) == sizeof(void*), "Invalid LONG_PTR size");

        auto window = reinterpret_cast<CoreWindow*>(::GetWindowLongPtrW(handle, GWLP_USERDATA));

        if (window == nullptr)
        {
            CORE_ASSERT(message == WM_NCCREATE);

            //
            // LPARAM contains value passed to CreateWindowEx (to 'this').
            //
            auto create_params = reinterpret_cast<::CREATESTRUCTW*>(lparam);
            window = reinterpret_cast<CoreWindow*>(create_params->lpCreateParams);

            //
            // Attach pointer to CoreWindow to handle.
            //
            ::SetWindowLongPtrW(
                handle,
                GWLP_USERDATA,
                reinterpret_cast<::LONG_PTR>(window));
        }

        if (window != nullptr)
        {
            switch (message)
            {
            case WM_CHAR:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    return 0;
                }

            case WM_SYSCHAR:
                {
                    if ((HIWORD(lparam) & 0x2000) != 0 && wparam == VK_SPACE)
                    {
                        break;
                    }
                    else
                    {
                        return 0;
                    }

                    break;
                }

            case WM_SYSKEYDOWN:
                {
                    if (wparam != VK_F4 && wparam != VK_SPACE)
                    {
                        ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    }

                    break;
                }

            case WM_KEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYUP:
            case WM_LBUTTONDBLCLK:
            case WM_LBUTTONDOWN:
            case WM_MBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
            case WM_RBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
            case WM_XBUTTONDBLCLK:
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP:
            case WM_LBUTTONUP:
            case WM_MBUTTONUP:
            case WM_RBUTTONUP:
            case WM_NCMOUSEMOVE:
            case WM_MOUSEMOVE:
            case WM_MOUSEWHEEL:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    return 0;
                }

            case WM_INPUT:
                {
                    ::UINT size;
                    ::GetRawInputData(
                        reinterpret_cast<::HRAWINPUT>(lparam),
                        RID_INPUT,
                        nullptr,
                        &size,
                        sizeof(RAWINPUTHEADER));

                    std::vector<uint8_t> rawdata(size, 0);

                    if (::GetRawInputData(
                        reinterpret_cast<::HRAWINPUT>(lparam),
                        RID_INPUT,
                        rawdata.data(),
                        &size,
                        sizeof(::RAWINPUTHEADER)) == size)
                    {
                        const auto raw = reinterpret_cast<const::RAWINPUT* const>(rawdata.data());

                        if (raw->header.dwType == RIM_TYPEMOUSE)
                        {
                            const auto is_absolute = (raw->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE) == MOUSE_MOVE_ABSOLUTE;

                            if (is_absolute)
                            {
                                ProcessMessage(window, handle, message, wparam, lparam, 0, 0, MOUSE_MOVE_ABSOLUTE);
                                return 1;
                            }

                            const int32_t relative_x = raw->data.mouse.lLastX;
                            const int32_t relative_y = raw->data.mouse.lLastY;

                            ProcessMessage(window, handle, message, wparam, lparam, relative_x, relative_y, MOUSE_MOVE_RELATIVE);
                            return 1;
                        }
                    }

                    break;
                }

            case WM_SHOWWINDOW:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_SIZE:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_SIZING:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_ENTERSIZEMOVE:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_EXITSIZEMOVE:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_MOVE:
                {
                    const auto new_x = static_cast<int32_t>(static_cast<int16_t>(LOWORD(lparam)));
                    const auto new_y = static_cast<int32_t>(static_cast<int16_t>(HIWORD(lparam)));

                    const auto is_minimized = (new_x == -32000) && (new_y == -32000);

                    if (is_minimized)
                    {
                        m_EventHandler->OnWindowMoved(window, new_x, new_y);
                        return 0;
                    }

                    break;
                }

#if WINVER > 0x502
            case WM_DWMCOMPOSITIONCHANGED:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }
#endif

            case WM_ACTIVATE:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_ACTIVATEAPP:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    break;
                }

            case WM_PAINT:
                {
                    ::PAINTSTRUCT ps{};
                    ::BeginPaint(handle, &ps);
                    ::EndPaint(handle, &ps);

                    m_EventHandler->OnSystemPaint(window);
                    break;
                }

            case WM_ERASEBKGND:
                {
                    return 1;
                }

            case WM_DESTROY:
                {
                    return 0;
                }

            case WM_CLOSE:
                {
                    ProcessMessage(window, handle, message, wparam, lparam, 0, 0, 0);
                    return 0;
                }

            case WM_POWERBROADCAST:
                {
                    switch (wparam)
                    {
                    case PBT_APMQUERYSUSPEND:
                        {
                            m_EventHandler->OnSuspending();
                            return TRUE;
                        }

                    case PBT_APMRESUMESUSPEND:
                        {
                            m_EventHandler->OnResuming();
                            return TRUE;
                        }
                    }
                    break;
                }

            case WM_GETDLGCODE:
                {
                    return DLGC_WANTALLKEYS;
                }

            case WM_CREATE:
                {
                    return 0;
                }
            }
        }

        return ::DefWindowProcW(handle, message, wparam, lparam);
    }

    ::LRESULT CoreApplication::ProcessMessage(CoreWindow* window, ::HWND handle, ::UINT message, ::WPARAM wparam, ::LPARAM lparam, int32_t mouseX, int32_t mouseY, ::UINT flags) noexcept
    {
        (void)handle;

        if (window != nullptr)
        {
            switch (message)
            {
            case WM_CHAR:
                {
                    const auto character = static_cast<char32_t>(wparam);
                    const auto is_repeat = (lparam & UINT32_C(0x40000000)) != 0;

                    m_EventHandler->OnKeyChar(character, is_repeat);
                    return 0;
                }

            case WM_SYSKEYDOWN:
            case WM_KEYDOWN:
                {
                    auto key = static_cast<int32_t>(wparam);
                    auto actual_key = key;

                    bool is_repeat = (lparam & UINT32_C(0x40000000)) != 0;

                    switch (key)
                    {
                    case VK_MENU:
                        {
                            if ((lparam & UINT32_C(0x1000000)) == 0)
                            {
                                actual_key = VK_LMENU;
                                is_repeat = (m_ModifierKeyMask & ModifierKeyMask::LeftAlt) == ModifierKeyMask::LeftAlt;
                                if (is_repeat)
                                {
                                    m_ModifierKeyMask |= ModifierKeyMask::LeftAlt;
                                }
                            }
                            else
                            {
                                actual_key = VK_RMENU;
                                is_repeat = (m_ModifierKeyMask & ModifierKeyMask::RightAlt) == ModifierKeyMask::RightAlt;
                                if (is_repeat)
                                {
                                    m_ModifierKeyMask |= ModifierKeyMask::RightAlt;
                                }
                            }
                            break;
                        }

                    case VK_CONTROL:
                        {
                            if ((lparam & UINT32_C(0x1000000)) == 0)
                            {
                                actual_key = VK_LCONTROL;
                                is_repeat = (m_ModifierKeyMask & ModifierKeyMask::LeftControl) == ModifierKeyMask::LeftControl;
                                if (is_repeat)
                                {
                                    m_ModifierKeyMask |= ModifierKeyMask::LeftControl;
                                }
                            }
                            else
                            {
                                actual_key = VK_RCONTROL;
                                is_repeat = (m_ModifierKeyMask & ModifierKeyMask::RightControl) == ModifierKeyMask::RightControl;
                                if (is_repeat)
                                {
                                    m_ModifierKeyMask |= ModifierKeyMask::RightControl;
                                }
                            }
                            break;
                        }

                    case VK_SHIFT:
                        {
                            actual_key = ::MapVirtualKeyW((lparam & UINT32_C(0x00FF0000)) >> 16, MAPVK_VSC_TO_VK_EX);
                            auto modifier_key = (actual_key == VK_LSHIFT) ? ModifierKeyMask::LeftShift : ModifierKeyMask::RightShift;

                            is_repeat = (m_ModifierKeyMask & modifier_key) == modifier_key;

                            if (is_repeat)
                            {
                                m_ModifierKeyMask |= modifier_key;
                            }

                            if ((lparam & UINT32_C(0x1000000)) == 0)
                            {
                                actual_key = VK_LSHIFT;
                                is_repeat = (m_ModifierKeyMask & ModifierKeyMask::LeftShift) == ModifierKeyMask::LeftShift;
                                if (is_repeat)
                                {
                                    m_ModifierKeyMask |= ModifierKeyMask::LeftShift;
                                }
                            }
                            else
                            {
                                actual_key = VK_RSHIFT;
                                is_repeat = (m_ModifierKeyMask & ModifierKeyMask::RightShift) == ModifierKeyMask::RightShift;
                                if (is_repeat)
                                {
                                    m_ModifierKeyMask |= ModifierKeyMask::RightShift;
                                }
                            }
                            break;
                        }

                    default:
                        {
                            break;
                        }
                    }

                    const auto char_code = static_cast<char32_t>(::MapVirtualKeyW(key, MAPVK_VK_TO_CHAR));

                    const auto result = m_EventHandler->OnKeyDown(actual_key, char_code, is_repeat);

                    if (result != 0 || message != WM_SYSKEYDOWN)
                    {
                        return 0;
                    }

                    break;
                }

            case WM_SYSKEYUP:
            case WM_KEYUP:
                {
                    auto key = static_cast<int32_t>(wparam);
                    auto actual_key = key;

                    bool is_repeat = (lparam & UINT32_C(0x40000000)) != 0;

                    switch (key)
                    {
                    case VK_MENU:
                        {
                            if ((lparam & UINT32_C(0x1000000)) == 0)
                            {
                                actual_key = VK_LMENU;
                                m_ModifierKeyMask &= ~ModifierKeyMask::LeftAlt;
                            }
                            else
                            {
                                actual_key = VK_RMENU;
                                m_ModifierKeyMask &= ~ModifierKeyMask::RightAlt;
                            }

                            break;
                        }

                    case VK_CONTROL:
                        {
                            if ((lparam & UINT32_C(0x1000000)) == 0)
                            {
                                actual_key = VK_LCONTROL;
                                m_ModifierKeyMask &= ~ModifierKeyMask::LeftControl;
                            }
                            else
                            {
                                actual_key = VK_RCONTROL;
                                m_ModifierKeyMask &= ~ModifierKeyMask::RightControl;
                            }

                            break;
                        }

                    case VK_SHIFT:
                        {
                            actual_key = ::MapVirtualKeyW((lparam & UINT32_C(0x00FF0000)) >> 16, MAPVK_VSC_TO_VK_EX);

                            if (actual_key == VK_LSHIFT)
                            {
                                m_ModifierKeyMask &= ~ModifierKeyMask::LeftShift;
                            }
                            else if (actual_key == VK_RSHIFT)
                            {
                                m_ModifierKeyMask &= ~ModifierKeyMask::RightShift;
                            }

                            break;
                        }

                    default:
                        {
                            break;
                        }
                    }

                    auto char_code = static_cast<char32_t>(::MapVirtualKeyW(key, MAPVK_VK_TO_CHAR));

                    const auto result = m_EventHandler->OnKeyUp(actual_key, char_code, is_repeat);

                    if (result || message != WM_SYSKEYUP)
                    {
                        return 0;
                    }

                    break;
                }

            case WM_LBUTTONDBLCLK:
            case WM_LBUTTONDOWN:
                {
                    if (message == WM_LBUTTONDOWN)
                    {
                        m_EventHandler->OnMouseDown(window, MouseButton::Left);
                    }
                    else
                    {
                        m_EventHandler->OnMouseDoubleClick(window, MouseButton::Left);
                    }

                    return 0;
                }

            case WM_MBUTTONDBLCLK:
            case WM_MBUTTONDOWN:
                {
                    if (message == WM_MBUTTONDOWN)
                    {
                        m_EventHandler->OnMouseDown(window, MouseButton::Middle);
                    }
                    else
                    {
                        m_EventHandler->OnMouseDoubleClick(window, MouseButton::Middle);
                    }

                    return 0;
                }

            case WM_RBUTTONDBLCLK:
            case WM_RBUTTONDOWN:
                {
                    if (message == WM_RBUTTONDOWN)
                    {
                        m_EventHandler->OnMouseDown(window, MouseButton::Right);
                    }
                    else
                    {
                        m_EventHandler->OnMouseDoubleClick(window, MouseButton::Right);
                    }

                    return 0;
                }

            case WM_XBUTTONDBLCLK:
            case WM_XBUTTONDOWN:
                {
                    auto button = (HIWORD(wparam) & XBUTTON1) ? MouseButton::Thumb1 : MouseButton::Thumb2;

                    auto result = false;
                    if (message == WM_MBUTTONDOWN)
                    {
                        result = m_EventHandler->OnMouseDown(window, button);
                    }
                    else
                    {
                        result = m_EventHandler->OnMouseDoubleClick(window, button);
                    }

                    return result ? 0 : 1;
                }

            case WM_LBUTTONUP:
                {
                    return m_EventHandler->OnMouseUp(window, MouseButton::Left) ? 0 : 1;
                }

            case WM_MBUTTONUP:
                {
                    return m_EventHandler->OnMouseUp(window, MouseButton::Middle) ? 0 : 1;
                }

            case WM_RBUTTONUP:
                {
                    return m_EventHandler->OnMouseUp(window, MouseButton::Right) ? 0 : 1;
                }

            case WM_XBUTTONUP:
                {
                    auto button = (HIWORD(wparam) & XBUTTON1) ? MouseButton::Thumb1 : MouseButton::Thumb2;
                    return m_EventHandler->OnMouseUp(window, button) ? 0 : 1;
                }

            case WM_INPUT:
                {
                    if (flags == MOUSE_MOVE_RELATIVE)
                    {
                        m_EventHandler->OnMouseMoveRaw(mouseX, mouseY);
                    }
                    else
                    {
                        m_EventHandler->OnMouseMove();
                    }

                    return 0;
                }

            case WM_NCMOUSEMOVE:
            case WM_MOUSEMOVE:
                {
                    auto result = false;

                    //if (!_is_using_high_precision_mouse_input)
                    {
                        result = m_EventHandler->OnMouseMove();
                    }

                    return result ? 0 : 1;
                }

            case WM_MOUSEWHEEL:
                {
                    const auto spin_factor = 1.0F / 120.0F;
                    const auto wheel_delta = GET_WHEEL_DELTA_WPARAM(wparam);
                    const auto result = m_EventHandler->OnMouseWheel(static_cast<float>(wheel_delta) * spin_factor);
                    return result ? 0 : 1;
                }

            case WM_ACTIVATE:
                {
                    auto activation_type = CoreWindowActivation::Deactivate;

                    if (LOWORD(wparam) & WA_ACTIVE)
                    {
                        activation_type = CoreWindowActivation::Activate;
                    }
                    else if (LOWORD(wparam) & WA_CLICKACTIVE)
                    {
                        activation_type = CoreWindowActivation::ActivateByMouse;
                    }

                    auto result = false;
                    result = m_EventHandler->OnWindowActivated(window, activation_type);
                    return result ? 0 : 1;
                }

            case WM_ACTIVATEAPP:
                {
                    m_EventHandler->OnApplicationActivated(!!wparam);
                    break;
                }

            case WM_CLOSE:
                {
                    m_EventHandler->OnWindowClose(window);
                    return 0;
                }

            case WM_SIZE:
                {
                    auto sizeX = static_cast<int32_t>(static_cast<int16_t>(LOWORD(lparam)));
                    auto sizeY = static_cast<int32_t>(static_cast<int16_t>(HIWORD(lparam)));

                    const auto wasMinimized = (wparam == SIZE_MINIMIZED);
                    const auto result = m_EventHandler->OnWindowSizeChanged(
                        window,
                        sizeX,
                        sizeY,
                        wasMinimized
                    );
                    
                    break;
                }

            case WM_SIZING:
                {
                    m_EventHandler->OnWindowSizing(window);
                    break;
                }

            case WM_ENTERSIZEMOVE:
                {
                    m_EventHandler->OnWindowEnterSizeMove(window);
                    break;
                }

            case WM_EXITSIZEMOVE:
                {
                    m_EventHandler->OnWindowExitSizeMove(window);

                    break;
                }
            }
        }

        return 0;
    }

    ::LRESULT CALLBACK CoreApplication::ApplicationWindowProc(::HWND handle, ::UINT message, ::WPARAM wparam, ::LPARAM lparam) noexcept
    {
        //
        // Delegate handling message to current application.
        //
        CORE_ASSERT(CoreApplication::Current != nullptr);
        return CoreApplication::Current->ProcessMessage(handle, message, wparam, lparam);
    }
}
//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/CoreEventHandler.hxx>

namespace Core
{
    CoreEventHandler::CoreEventHandler() noexcept = default;

    CoreEventHandler::~CoreEventHandler() noexcept = default;

    bool CoreEventHandler::OnKeyDown(uint32_t keyCode, char32_t character, bool isRepeat) noexcept
    {
        (void)keyCode;
        (void)character;
        (void)isRepeat;
        return false;
    }

    bool CoreEventHandler::OnKeyUp(uint32_t keyCode, char32_t character, bool isRepeat) noexcept
    {
        (void)keyCode;
        (void)character;
        (void)isRepeat;
        return false;
    }

    bool CoreEventHandler::OnKeyChar(char32_t character, bool isRepeat) noexcept
    {
        (void)character;
        (void)isRepeat;
        return false;
    }

    bool CoreEventHandler::OnMouseDown(CoreWindow* window, MouseButton button) noexcept
    {
        (void)window;
        (void)button;
        return false;
    }

    bool CoreEventHandler::OnMouseUp(CoreWindow* window, MouseButton button) noexcept
    {
        (void)window;
        (void)button;
        return false;
    }

    bool CoreEventHandler::OnMouseDoubleClick(CoreWindow* window, MouseButton button) noexcept
    {
        (void)window;
        (void)button;
        return false;
    }

    bool CoreEventHandler::OnMouseWheel(float delta) noexcept
    {
        (void)delta;
        return false;
    }

    bool CoreEventHandler::OnMouseMoveRaw(int32_t x, int32_t y) noexcept
    {
        (void)x;
        (void)y;
        return false;
    }

    bool CoreEventHandler::OnMouseMove() noexcept
    {
        return false;
    }

    bool CoreEventHandler::OnControllerAnalog(ControllerButton button, int32_t controllerId, float value) noexcept
    {
        (void)button;
        (void)controllerId;
        (void)value;
        return false;
    }

    bool CoreEventHandler::OnControllerButtonPressed(ControllerButton button, int32_t controllerId, bool isRepeat) noexcept
    {
        (void)button;
        (void)controllerId;
        (void)isRepeat;
        return false;
    }

    bool CoreEventHandler::OnControllerButtonReleased(ControllerButton button, int32_t controllerId, bool isRepeat) noexcept
    {
        (void)button;
        (void)controllerId;
        (void)isRepeat;
        return false;
    }

    void CoreEventHandler::OnSystemPaint(CoreWindow* window) noexcept
    {
        (void)window;
    }

    bool CoreEventHandler::OnWindowActivated(CoreWindow* window, CoreWindowActivation activationType) noexcept
    {
        (void)window;
        (void)activationType;
        return false;
    }

    bool CoreEventHandler::OnApplicationActivated(bool isActive) noexcept
    {
        (void)isActive;
        return false;
    }

    void CoreEventHandler::OnWindowClose(CoreWindow* window) noexcept
    {
        (void)window;
    }

    void CoreEventHandler::OnWindowMoved(CoreWindow* window, int32_t x, int32_t y) noexcept
    {
        (void)window;
        (void)x;
        (void)y;
    }

    void CoreEventHandler::OnSuspending() noexcept
    {
    }

    void CoreEventHandler::OnResuming() noexcept
    {
    }

    bool CoreEventHandler::OnWindowSizeChanged(CoreWindow* window, int32_t width, int32_t height, bool wasMinimized) noexcept
    {
        (void)window;
        (void)width;
        (void)height;
        (void)wasMinimized;
        return false;
    }

    bool CoreEventHandler::OnWindowEnterSizeMove(CoreWindow* window) noexcept
    {
        (void)window;
        return false;
    }

    void CoreEventHandler::OnWindowExitSizeMove(CoreWindow* window) noexcept
    {
        (void)window;
    }

    void CoreEventHandler::OnWindowSizing(CoreWindow* window) noexcept
    {
        (void)window;
    }
}
#ifndef INCLUDED_CORE_COREEVENTHANDLER_HXX
#define INCLUDED_CORE_COREEVENTHANDLER_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core/Reference.hxx>
#include <Core/CoreWindow.hxx>
#include <Core/Common.hxx>

namespace Core
{
    enum class MouseButton
    {
        Left,
        Middle,
        Right,
        Thumb1,
        Thumb2
    };

    enum class ControllerButton
    {
        LeftAnalogX,
        LeftAnalogY,
        RightAnalogX,
        RightAnalogY,
        TriggerAnalogLeft,
        TriggerAnalogRight,
        TriggerTresholdLeft,
        TriggerTresholdRight,

        FaceButtonBottom,
        FaceButtonRight,
        FaceButtonLeft,
        FaceButtonTop,

        ShoulderLeft,
        ShoulderRight,
        SpecialLeft,
        SpecialRight,
        ThumbLeft,
        ThumbRight,

        DPadUp,
        DPadDown,
        DPadLeft,
        DPadRight,

        LeftStickUp,
        LeftStickDown,
        LeftStickLeft,
        LeftStickRight,

        RightStickUp,
        RightStickDown,
        RightStickLeft,
        RightStickRight,

        Unknown,
    };

    enum class CoreWindowActivation
    {
        Activate,
        ActivateByMouse,
        Deactivate,
    };

    using CoreEventHandlerRef = Reference<class CoreEventHandler>;
    class CoreEventHandler : public Object
    {
    public:
        CoreEventHandler() noexcept;
        virtual ~CoreEventHandler() noexcept;

    public:
        virtual bool OnKeyDown(uint32_t keyCode, char32_t character, bool isRepeat) noexcept;
        virtual bool OnKeyUp(uint32_t keyCode, char32_t character, bool isRepeat) noexcept;
        virtual bool OnKeyChar(char32_t character, bool isRepeat) noexcept;

    public:
        virtual bool OnMouseDown(CoreWindow* window, MouseButton button) noexcept;
        virtual bool OnMouseUp(CoreWindow* window, MouseButton button) noexcept;
        virtual bool OnMouseDoubleClick(CoreWindow* window, MouseButton button) noexcept;
        virtual bool OnMouseWheel(float delta) noexcept;
        virtual bool OnMouseMoveRaw(int32_t x, int32_t y) noexcept;
        virtual bool OnMouseMove() noexcept;

    public:
        virtual bool OnControllerAnalog(ControllerButton button, int32_t controllerId, float value) noexcept;
        virtual bool OnControllerButtonPressed(ControllerButton button, int32_t controllerId, bool isRepeat) noexcept;
        virtual bool OnControllerButtonReleased(ControllerButton button, int32_t controllerId, bool isRepeat) noexcept;

    public:
        virtual void OnSystemPaint(CoreWindow* window) noexcept;
        virtual bool OnWindowActivated(CoreWindow* window, CoreWindowActivation activationType) noexcept;
        virtual bool OnApplicationActivated(bool isActive) noexcept;
        virtual void OnWindowClose(CoreWindow* window) noexcept;
        virtual void OnWindowMoved(CoreWindow* window, int32_t x, int32_t y) noexcept;
        virtual void OnSuspending() noexcept;
        virtual void OnResuming() noexcept;

    public:
        virtual bool OnWindowSizeChanged(CoreWindow* window, int32_t width, int32_t height, bool wasMinimized) noexcept;
        virtual bool OnWindowEnterSizeMove(CoreWindow* window) noexcept;
        virtual void OnWindowExitSizeMove(CoreWindow* window) noexcept;
        virtual void OnWindowSizing(CoreWindow* window) noexcept;
    };
}

#endif // INCLUDED_CORE_COREEVENTHANDLER_HXX
#ifndef INCLUDED_CORE_COREAPPLICATION_HXX
#define INCLUDED_CORE_COREAPPLICATION_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core/Reference.hxx>
#include <Core/CoreWindow.hxx>
#include <Core/CoreEventHandler.hxx>

namespace Core
{

    enum class ModifierKeyMask
    {
        None = 0,
        LeftShift = 1 << 0,
        LeftControl = 1 << 1,
        LeftAlt = 1 << 2,
        LeftCommand = 1 << 3,
        RightShift = 1 << 4,
        RightControl = 1 << 5,
        RightAlt = 1 << 6,
        RightCommand = 1 << 7,
    };
    CORE_ENUM_CLASS_FLAGS(ModifierKeyMask);

    using CoreApplicationRef = Reference<class CoreApplication>;
    class CoreApplication final : public Object
    {
        friend class CoreWindow;

    private:
        CoreEventHandlerRef m_EventHandler;
        ModifierKeyMask m_ModifierKeyMask;

    public:
        static CoreApplication* Current;

    public:
        CoreApplication() noexcept;
        virtual ~CoreApplication() noexcept;

    public:
        CoreWindowRef MakeWindow(const CoreWindowDesc& desc) noexcept;

        void Tick(float deltaTime) noexcept;

        inline void SetEventHandler(const CoreEventHandlerRef& eventHandler) noexcept
        {
            m_EventHandler = eventHandler;
        }

    public:
        static CoreApplicationRef MakeApplication() noexcept;

        static void PumpMessages() noexcept;

    private:
        ::LRESULT ProcessMessage(::HWND handle, ::UINT message, ::WPARAM wparam, ::LPARAM lparam) noexcept;
        ::LRESULT ProcessMessage(CoreWindow* window, ::HWND handle, ::UINT message, ::WPARAM wparam, ::LPARAM lparam, int32_t mouseX, int32_t mouseY, ::UINT flags) noexcept;

    private:
        static ::LRESULT CALLBACK ApplicationWindowProc(::HWND handle, ::UINT message, ::WPARAM wparam, ::LPARAM lparam) noexcept;
    };
}

#endif // INCLUDED_CORE_COREAPPLICATION_HXX
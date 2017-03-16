#ifndef INCLUDED_CORE_COREWINDOW_HXX
#define INCLUDED_CORE_COREWINDOW_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core/Reference.hxx>

namespace Core
{
    struct CoreWindowDesc
    {
        int32_t Width;
        int32_t Height;
    };

    using CoreWindowRef = Reference<class CoreWindow>;
    class CoreWindow final : public Object
    {
        friend class CoreApplication;

    public:
        CoreWindow(const CoreWindowDesc& desc) noexcept;
        virtual ~CoreWindow() noexcept;

    public:
        void Show() noexcept;
        void Hide() noexcept;
        void SetFullscreen(bool value) noexcept;
        void SetEnabled(bool value) noexcept;
        inline void* GetHandle() const noexcept
        {
            return reinterpret_cast<void*>(m_Handle);
        }
        void SetText(const char* text) noexcept;
        void Destroy() noexcept;

    private:
        static void TryRegisterWindowClass(::HINSTANCE instance, ::HICON icon) noexcept;

    private:
        CoreWindowDesc m_Desc;
        ::HWND m_Handle;
    };
}

#endif // INCLUDED_CORE_COREWINDOW_HXX
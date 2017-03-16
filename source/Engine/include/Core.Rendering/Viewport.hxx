#ifndef INCLUDED_CORE_RENDERING_VIEWPORT_HXX
#define INCLUDED_CORE_RENDERING_VIEWPORT_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core.Rendering/Resource.hxx>

namespace Core::Rendering
{
    //
    // Forward declaration.
    //
    class RenderSystem;

    using ViewportRef = Reference<class Viewport>;
    class Viewport : public Resource
    {
        friend class RenderSystem;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilTexture;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
        uint32_t m_Width;
        uint32_t m_Height;

    public:
        Viewport(RenderSystem* renderSystem, void* windowHandle, uint32_t width, uint32_t height, bool isFullscreen) noexcept;
        virtual ~Viewport() noexcept;

    private:
        void Resize(uint32_t width, uint32_t height, bool isFullscreen) noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_VIEWPORT_HXX
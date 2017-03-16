//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/Viewport.hxx>
#include <Core.Rendering/RenderSystem.hxx>

namespace Core::Rendering
{
    Viewport::Viewport(RenderSystem* renderSystem, void* windowHandle, uint32_t width, uint32_t height, bool isFullscreen) noexcept
        : m_RenderSystem{ renderSystem }
        , m_SwapChain{}
        , m_RenderTargetView{}
        , m_DepthStencilTexture{}
        , m_DepthStencilView{}
        , m_Width{ width }
        , m_Height{ height }
    {
        auto device = m_RenderSystem->m_Device;

        //
        // Describe swap chain.
        //
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.Width = m_Width;
        sd.BufferDesc.Height = m_Height;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        sd.Flags = 0;
        sd.OutputWindow = reinterpret_cast<::HWND>(windowHandle);
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Windowed = isFullscreen ? FALSE : TRUE;

        //
        // Create swap chain.
        //
        DX::Ensure(m_RenderSystem->m_DxgiFactory->CreateSwapChain(
            device.Get(),
            &sd,
            m_SwapChain.GetAddressOf()
        ));

        //
        // Disable Alt+Enter.
        //
        // Note:
        //      On my laptop, without hitting Alt+Enter swap chain is not presented :/
        //
        //DX::Ensure(m_RenderSystem->m_DxgiFactory->MakeWindowAssociation(sd.OutputWindow, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES));

        CORE_TRACE_MESSAGE(Info, "[D3D11] Created swap chain");

        //
        // Initially resize viewport to desired size.
        //
        Resize(m_Width, m_Height, false);
    }

    Viewport::~Viewport() noexcept
    {
        //
        // Make sure that we switch to windowed mode before destoying viewport.
        //
        // On my laptop this caused occasional glitches with GPU. However, it's already old junk :)
        //
        DX::Ensure(m_SwapChain->SetFullscreenState(FALSE, nullptr));
        
        CORE_TRACE_MESSAGE(Info, "[D3D11] Destroying viewport");
    }

    void Viewport::Resize(uint32_t width, uint32_t height, bool isFullscreen) noexcept
    {
        auto device = m_RenderSystem->m_Device;

        (void)isFullscreen;

        //
        // Update to new viewport  size.
        //
        m_Width = width;
        m_Height = height;

        CORE_TRACE_MESSAGE(Info, "[D3D11] Resizing viewport (%u x %u)", width, height);

        //
        // Deallocate previous resources.
        //
        m_RenderTargetView = nullptr;
        m_DepthStencilView = nullptr;
        m_DepthStencilTexture = nullptr;

        //
        // Resize viewport buffers.
        //
        DX::Ensure(m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

        //
        // Get back buffer texture.
        //
        Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer{};
        DX::Ensure(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBuffer.GetAddressOf()));

        //
        // And create render target view.
        //
        DX::Ensure(device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RenderTargetView.GetAddressOf()));

        backBuffer = nullptr;

        //
        // Also we want to create depth stencil buffer.
        //
        {
            //
            // Describe that buffer.
            //
            D3D11_TEXTURE2D_DESC desc{};
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = 1;
            desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            //
            // Create texture for it.
            //
            DX::Ensure(device->CreateTexture2D(&desc, nullptr, m_DepthStencilTexture.GetAddressOf()));

            CORE_TRACE_MESSAGE(Info, "[D3D11] Created Depth Stencil buffer");

            //
            // And depth stencil view.
            //
            DX::Ensure(device->CreateDepthStencilView(m_DepthStencilTexture.Get(), nullptr, m_DepthStencilView.GetAddressOf()));

            CORE_TRACE_MESSAGE(Info, "[D3D11] Created DSV");
        }

        auto context = m_RenderSystem->m_ImmediateContext;

        // Bind the render target view and depth/stencil view to the pipeline.

        context->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());


        // Set the viewport transform.

        D3D11_VIEWPORT viewport;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = static_cast<float>(m_Width);
        viewport.Height = static_cast<float>(m_Height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        //
        // And set viewport information.
        //
        context->RSSetViewports(1, &viewport);
    }
}
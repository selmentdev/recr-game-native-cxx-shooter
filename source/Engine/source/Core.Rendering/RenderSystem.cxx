//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/RenderSystem.hxx>
#include <codecvt>
#include <cinttypes>

//
// Meh.
//
#pragma comment(lib, "d3d11.lib")

namespace Core::Rendering
{
    //
    // Just to easily access current render system.
    //
    // In some places I started to put used render system into resources. But **how** many times
    // game engine / editor would setup more than one render system anyway?
    //
    RenderSystem* RenderSystem::Current{ nullptr };

    RenderSystem::RenderSystem() noexcept
    {
        InitializeDirect3D();
    }

    RenderSystem::~RenderSystem() noexcept
    {
        CORE_TRACE_MESSAGE(Info, "[D3D11] Destroy render system");
    }

    RenderSystemRef RenderSystem::MakeRenderSystem() noexcept
    {
        //
        // Currently, render system is implemented in D3D11 API.
        //
        CORE_ASSERT_MSG(RenderSystem::Current == nullptr, "Render system must not be initialized twice");
        auto renderSystem = MakeRef<RenderSystem>();

        RenderSystem::Current = renderSystem.Get();
        return renderSystem;
    }

    ViewportRef RenderSystem::MakeViewport(void* windowHandle, uint32_t width, uint32_t height, bool isFullscreen) noexcept
    {
        return MakeRef<Viewport>(this, windowHandle, width, height, isFullscreen);
    }

    void RenderSystem::ResizeViewport(const ViewportRef& viewport, uint32_t width, uint32_t height, bool isFullscreen) noexcept
    {
        viewport->Resize(width, height, isFullscreen);
    }

    void RenderSystem::BeginDrawViewport(const ViewportRef& viewport) noexcept
    {
        //
        // Setup viewport description.
        //
        ::D3D11_VIEWPORT desc{};
        desc.TopLeftX = 0.0F;
        desc.TopLeftY = 0.0F;
        desc.Width = static_cast<float>(viewport->m_Width);
        desc.Height = static_cast<float>(viewport->m_Height);
        desc.MinDepth = 0.0F;
        desc.MaxDepth = 1.0F;

        m_ImmediateContext->RSSetViewports(1, &desc);

        //
        // Bind viewport render targets.
        //
        m_ImmediateContext->OMSetRenderTargets(
            1,
            viewport->m_RenderTargetView.GetAddressOf(),
            viewport->m_DepthStencilView.Get()
        );

        FLOAT color[4] = {
            0.0F,
            0.0F,
            0.0F,
            0.0F
        };

        //
        // Clear render target.
        //
        m_ImmediateContext->ClearRenderTargetView(viewport->m_RenderTargetView.Get(), color);
        m_ImmediateContext->ClearDepthStencilView(viewport->m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0F, 0);

    }

    void RenderSystem::EndDrawViewport(const ViewportRef& viewport, bool present, uint32_t interval) noexcept
    {
        if (present)
        {
            //
            // Present swap chain.
            //
            DX::Ensure(viewport->m_SwapChain->Present(interval, 0));
        }
    }

    GraphicsPipelineStateRef RenderSystem::MakeGraphicsPipelineState(const GraphicsPipelineStateDesc& desc) noexcept
    {
        return MakeRef<GraphicsPipelineState>(this, desc);
    }

    void RenderSystem::Tick(float deltaTime) noexcept
    {
        (void)deltaTime;
    }

    VertexBufferRef RenderSystem::MakeVertexBuffer(const BufferDesc& desc) noexcept
    {
        return MakeRef<VertexBuffer>(this, desc);
    }

    IndexBufferRef RenderSystem::MakeIndexBuffer(const BufferDesc& desc) noexcept
    {
        return MakeRef<IndexBuffer>(this, desc);
    }

    UniformBufferRef RenderSystem::MakeUniformBuffer(const BufferDesc& desc) noexcept
    {
        return MakeRef<UniformBuffer>(this, desc);
    }

    OcclusionQueryRef RenderSystem::MakeOcclusionQuery() noexcept
    {
        return MakeRef<OcclusionQuery>(this);
    }

    CommandListRef RenderSystem::GetImmediateCommandList() noexcept
    {
        return m_DefaultImmediateContext;
    }

    CommandListRef RenderSystem::MakeCommandList() noexcept
    {
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{};

        m_Device->CreateDeferredContext(0, context.GetAddressOf());
        return MakeRef<CommandList>(this, context);
    }

    SamplerRef RenderSystem::MakeSampler(const SamplerDesc& desc) noexcept
    {
        return MakeRef<Sampler>(this, desc);
    }

    Texture2DRef RenderSystem::MakeTexture2D(const std::string& path) noexcept
    {
        return MakeRef<Texture2D>(this, path);
    }

    void RenderSystem::InitializeDirect3D() noexcept
    {
        //
        // Setup creation flags.
        //
        ::UINT uFlags{ 0 };

#ifndef NDEBUG
        //
        // For debug builds allow D3D11 to emit some debug messages.
        //
        uFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        //
        // Setup feature levels.
        //
        const D3D_FEATURE_LEVEL featureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1,
        };

        //
        // Create device.
        //
        DX::Ensure(D3D11CreateDevice(
            nullptr,                        // Use default adapter.
            D3D_DRIVER_TYPE_HARDWARE,       // Preffer hardware adapter.
            nullptr,                        // Don't use any software providers.
            uFlags,                         // Specify flags.
            featureLevels,                  // Feature levels.
            sizeof(featureLevels) / sizeof(featureLevels[0]),   // Number of feature levels,
            D3D11_SDK_VERSION,              // SDK version for this build.
            m_Device.GetAddressOf(),
            &m_CurrentFeatureLevel,
            m_ImmediateContext.GetAddressOf()
        ));

        CORE_TRACE_MESSAGE(Info, "[D3D11] Initialized at feature level: %04x", static_cast<uint32_t>(m_CurrentFeatureLevel));

        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice{};

        //
        // Get DXGI device.
        //
        DX::Ensure(m_Device.As(&dxgiDevice));
        {
            ::INT gpuThreadPriority{};

            dxgiDevice->GetGPUThreadPriority(&gpuThreadPriority);
            CORE_TRACE_MESSAGE(Info, "[D3D11] GPU Thread Priority: %d", gpuThreadPriority);
        }

        //
        // Get DXGI Adapter.
        //
        DX::Ensure(dxgiDevice->GetAdapter(m_DxgiAdapter.GetAddressOf()));
        {
            // Converter.
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};

            DXGI_ADAPTER_DESC desc{};
            DX::Ensure(m_DxgiAdapter->GetDesc(&desc));

            CORE_TRACE_MESSAGE(Info, "[D3D11] Adapter: DeviceID: %04x, VendorID: %04x, SubSysID: %04x",
                desc.DeviceId,
                desc.VendorId,
                desc.SubSysId
            );

            CORE_TRACE_MESSAGE(Info, "[D3D11] Adapter: Description: `%s`", converter.to_bytes(desc.Description).c_str());

            CORE_TRACE_MESSAGE(Info, "[D3D11] Adapter: SystemMemory: %" PRIu64 ", VideoMemory: %" PRIu64 ", SharedMemory: %" PRIu64,
                static_cast<uint64_t>(desc.DedicatedSystemMemory) >> 20,
                static_cast<uint64_t>(desc.DedicatedVideoMemory) >> 20,
                static_cast<uint64_t>(desc.SharedSystemMemory) >> 20
            );
        }

        //
        // And finally, DXGI factory.
        //
        DX::Ensure(m_DxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)m_DxgiFactory.GetAddressOf()));

        //
        // Then, make wrapper for immediate context.
        //
        m_DefaultImmediateContext = MakeRef<CommandList>(this, m_ImmediateContext);
    }
}
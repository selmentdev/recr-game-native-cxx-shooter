#ifndef INCLUDED_CORE_RENDERING_RENDERSYSTEM_HXX
#define INCLUDED_CORE_RENDERING_RENDERSYSTEM_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core/Reference.hxx>
#include <Core.Rendering/Common.hxx>
#include <Core.Rendering/Resource.hxx>
#include <Core.Rendering/Viewport.hxx>
#include <Core.Rendering/GraphicsPipelineState.hxx>
#include <Core.Rendering/Buffers.hxx>
#include <Core.Rendering/Query.hxx>
#include <Core.Rendering/CommandList.hxx>
#include <Core.Rendering/Sampler.hxx>
#include <Core.Rendering/Texture2D.hxx>

namespace Core::Rendering
{
    using RenderSystemRef = Reference<class RenderSystem>;
    class RenderSystem : public Object
    {
        friend class Texture2D;
        friend class Sampler;
        friend class Viewport;
        friend class GraphicsPipelineState;
        friend class VertexBuffer;
        friend class IndexBuffer;
        friend class UniformBuffer;
        friend class OcclusionQuery;
        friend class CommandList;

    private:
        Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_ImmediateContext;
        Microsoft::WRL::ComPtr<IDXGIFactory> m_DxgiFactory;
        Microsoft::WRL::ComPtr<IDXGIAdapter> m_DxgiAdapter;
        CommandListRef m_DefaultImmediateContext;

        D3D_FEATURE_LEVEL m_CurrentFeatureLevel;

    public:
        RenderSystem() noexcept;
        virtual ~RenderSystem() noexcept;

    public:
        RenderSystem(const RenderSystem&) = delete;
        RenderSystem& operator = (const RenderSystem&) = delete;

        //
        // Global access to current render system.
        //
    public:
        static RenderSystem* Current;

        //
        // Static constructor.
        //
    public:
        static RenderSystemRef MakeRenderSystem() noexcept;

        //
        // Render viewport support.
        //
    public:
        virtual ViewportRef MakeViewport(void* windowHandle, uint32_t width, uint32_t height, bool isFullscreen) noexcept;
        virtual void ResizeViewport(const ViewportRef& viewport, uint32_t width, uint32_t height, bool isFullscreen) noexcept;

        virtual void BeginDrawViewport(const ViewportRef& viewport) noexcept;
        virtual void EndDrawViewport(const ViewportRef& viewport, bool present, uint32_t interval) noexcept;

        //
        // Graphics Pipeline State.
        //
    public:
        virtual GraphicsPipelineStateRef MakeGraphicsPipelineState(const GraphicsPipelineStateDesc& desc) noexcept;

        //
        // Ticking.
        //
    public:
        virtual void Tick(float deltaTime) noexcept;

        //
        // Vertex buffer.
        //
    public:
        virtual VertexBufferRef MakeVertexBuffer(const BufferDesc& desc) noexcept;
        virtual IndexBufferRef MakeIndexBuffer(const BufferDesc& desc) noexcept;
        virtual UniformBufferRef MakeUniformBuffer(const BufferDesc& desc) noexcept;

        //
        // Occlusion query.
        //
    public:
        virtual OcclusionQueryRef MakeOcclusionQuery() noexcept;

        //
        // Command lists.
        //
    public:
        virtual CommandListRef GetImmediateCommandList() noexcept;
        virtual CommandListRef MakeCommandList() noexcept;

        //
        // Sampler.
        //
    public:
        virtual SamplerRef MakeSampler(const SamplerDesc& desc) noexcept;

        //
        // Texture
        //
    public:
        virtual Texture2DRef MakeTexture2D(const std::string& path) noexcept;

    private:
        void InitializeDirect3D() noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_RENDERSYSTEM_HXX
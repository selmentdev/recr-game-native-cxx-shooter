#ifndef INCLUDED_CORE_RENDERING_COMMANDLIST_HXX
#define INCLUDED_CORE_RENDERING_COMMANDLIST_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core.Rendering/Common.hxx>
#include <Core.Rendering/Resource.hxx>
#include <Core.Rendering/Query.hxx>
#include <Core.Rendering/GraphicsPipelineState.hxx>
#include <Core.Rendering/Buffers.hxx>
#include <Core.Rendering/Sampler.hxx>
#include <Core.Rendering/Texture2D.hxx>

namespace Core::Rendering
{
    enum class ShaderMask
    {
        Vertex = 1 << 0,
        Pixel = 1 << 1,
        Geometry = 1 << 2,
        Hull = 1 << 3,
        Domain = 1 << 4,
    };
    CORE_ENUM_CLASS_FLAGS(ShaderMask);


    class RenderSystem;

    using CommandListRef = Reference<class CommandList>;
    class CommandList : public Resource
    {
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_Context;

    public:
        CommandList(RenderSystem* renderSystem, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) noexcept;
        virtual ~CommandList() noexcept;

        //
        // Occlusion Query.
        //
    public:
        virtual void BeginOcclusionQuery(const OcclusionQueryRef& query) noexcept;
        virtual void EndOcclusionQuery(const OcclusionQueryRef& query) noexcept;
        virtual void GetOcclusionQueryResult(uint64_t& result, const OcclusionQueryRef& query) noexcept;

        //
        // Binding Graphics Pipeline State.
        //
    public:
        virtual void BindGraphicsPipelineState(const GraphicsPipelineStateRef& state) noexcept;

        //
        // Buffer binding.
        //
    public:
        virtual void BindUniformBuffer(ShaderMask mask, uint32_t index, const UniformBufferRef& buffer) noexcept;
        virtual void BindVertexBuffer(uint32_t index, const VertexBufferRef& buffer, uint32_t stride, uint32_t offset) noexcept;
        virtual void BindIndexBuffer(const IndexBufferRef& buffer, bool isNarrow) noexcept;

        //
        // Sampler.
        //
    public:
        virtual void BindSampler(ShaderMask mask, uint32_t index, const SamplerRef& sampler) noexcept;

        //
        // Texture.
        //
    public:
        void BindTexture2D(ShaderMask mask, uint32_t index, const Texture2DRef& texture) noexcept;

        //
        // Drawing.
        //
    public:
        virtual void DrawIndexed(uint32_t indexCount, uint32_t startLocation, uint32_t baseVertexLocation) noexcept;
        virtual void DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation) noexcept;
        virtual void Draw(uint32_t vertexCount, uint32_t startVertexLocation) noexcept;
        virtual void DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) noexcept;

        //
        // Uniform buffer content update.
        //
    public:
        virtual void UpdateUniformBuffer(const UniformBufferRef& buffer, const void* data, size_t size) noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_COMMANDLIST_HXX
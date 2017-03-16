//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/CommandList.hxx>

namespace Core::Rendering
{
    CommandList::CommandList(RenderSystem* renderSystem, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) noexcept
        : m_RenderSystem{ renderSystem }
        , m_Context{ context }
    {
    }

    CommandList::~CommandList() noexcept
    {
    }

    void CommandList::BeginOcclusionQuery(const OcclusionQueryRef& query) noexcept
    {
        m_Context->Begin(query->m_Query.Get());
    }

    void CommandList::EndOcclusionQuery(const OcclusionQueryRef& query) noexcept
    {
        m_Context->End(query->m_Query.Get());
    }

    void CommandList::GetOcclusionQueryResult(uint64_t& result, const OcclusionQueryRef& query) noexcept
    {
        result = 0;

        auto q = query->m_Query.Get();

        while (m_Context->GetData(q, &result, sizeof(result), 0) == S_FALSE)
        {
            ;
        }
    }

    void CommandList::BindGraphicsPipelineState(const GraphicsPipelineStateRef& state) noexcept
    {
        FLOAT blend[4] = { 1.0F, 1.0F, 1.0F, 1.0F };

        m_Context->OMSetBlendState(state->m_BlendState.Get(), blend, 0xff);
        m_Context->OMSetDepthStencilState(state->m_DepthStencilState.Get(), 0xff);
        m_Context->RSSetState(state->m_RasterizerState.Get());
        m_Context->IASetInputLayout(state->m_InputLayout.Get());
        m_Context->IASetPrimitiveTopology(state->m_PrimitiveTopology);

        m_Context->PSSetShader(state->m_PixelShader.Get(), nullptr, 0);
        m_Context->VSSetShader(state->m_VertexShader.Get(), nullptr, 0);
    }

    void CommandList::BindUniformBuffer(ShaderMask mask, uint32_t index, const UniformBufferRef& buffer) noexcept
    {
        auto native = buffer->m_Buffer.GetAddressOf();

        //
        // Try to set buffer in Pixel Shader.
        //
        if (!!(mask & ShaderMask::Pixel))
        {
            m_Context->PSSetConstantBuffers(index, 1, native);
        }

        //
        // Try to set buffer in Vertex Shader.
        //
        if (!!(mask & ShaderMask::Vertex))
        {
            m_Context->VSSetConstantBuffers(index, 1, native);
        }

        // TODO: Support other shader types.
    }

    void CommandList::BindVertexBuffer(uint32_t index, const VertexBufferRef& buffer, uint32_t stride, uint32_t offset) noexcept
    {
        auto native = buffer->m_Buffer.GetAddressOf();

        auto nativeStride = static_cast<::UINT>(stride);
        auto nativeOffset = static_cast<::UINT>(offset);

        m_Context->IASetVertexBuffers(index, 1, native, &nativeStride, &nativeOffset);
    }

    void CommandList::BindIndexBuffer(const IndexBufferRef& buffer, bool isNarrow) noexcept
    {
        auto native = buffer->m_Buffer.Get();

        m_Context->IASetIndexBuffer(native, isNarrow ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    }

    void CommandList::BindSampler(ShaderMask mask, uint32_t index, const SamplerRef& sampler) noexcept
    {
        auto native = sampler->m_Sampler.GetAddressOf();

        if (!!(mask & ShaderMask::Pixel))
        {
            m_Context->PSSetSamplers(index, 1, native);
        }

        // TODO: Support other shader types.
    }

    void CommandList::BindTexture2D(ShaderMask mask, uint32_t index, const Texture2DRef& texture) noexcept
    {
        auto native = texture->m_ShaderResourceView.GetAddressOf();

        if (!!(mask & ShaderMask::Pixel))
        {
            m_Context->PSSetShaderResources(index, 1, native);
        }

        // TODO: Support other shader types.
    }

    void CommandList::DrawIndexed(uint32_t indexCount, uint32_t startLocation, uint32_t baseVertexLocation) noexcept
    {
        m_Context->DrawIndexed(indexCount, startLocation, baseVertexLocation);
    }

    void CommandList::DrawIndexedInstanced(uint32_t indexCountPerInstance, uint32_t instanceCount, uint32_t startIndexLocation, uint32_t baseVertexLocation, uint32_t startInstanceLocation) noexcept
    {
        m_Context->DrawIndexedInstanced(indexCountPerInstance, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
    }

    void CommandList::Draw(uint32_t vertexCount, uint32_t startVertexLocation) noexcept
    {
        m_Context->Draw(vertexCount, startVertexLocation);
    }

    void CommandList::DrawInstanced(uint32_t vertexCountPerInstance, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation) noexcept
    {
        m_Context->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
    }

    void CommandList::UpdateUniformBuffer(const UniformBufferRef& buffer, const void* data, size_t size) noexcept
    {
        //
        // Just update uniform buffer content.
        //
        auto native = buffer->m_Buffer.Get();

        //
        // ...by map to system memory...
        //
        D3D11_MAPPED_SUBRESOURCE subresource{};
        DX::Ensure(m_Context->Map(native, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource));

        //
        // ...copy...
        //
        std::memcpy(subresource.pData, data, size);

        //
        // ...and unmap.
        //
        m_Context->Unmap(native, 0);
    }
}
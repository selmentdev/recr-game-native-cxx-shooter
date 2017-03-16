//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/GraphicsPipelineState.hxx>
#include <Core.Rendering/RenderSystem.hxx>
#include <Core.Diagnostics/Debug.hxx>

namespace Core::Rendering
{
    GraphicsPipelineState::GraphicsPipelineState(RenderSystem* renderSystem, const GraphicsPipelineStateDesc& desc) noexcept
        : m_RenderSystem{ renderSystem }
        , m_BlendState{}
        , m_DepthStencilState{}
        , m_RasterizerState{}
        , m_InputLayout{}
        , m_PixelShader{}
        , m_VertexShader{}
        , m_GeometryShader{}
        , m_HullShader{}
        , m_DomainShader{}
        , m_PrimitiveTopology{ desc.PrimitiveTopology }
    {
        //
        // We require those two shader types.
        //
        CORE_ASSERT(!desc.VertexShader.Code.empty());
        CORE_ASSERT(!desc.PixelShader.Code.empty());

        auto device = m_RenderSystem->m_Device;

        //
        // Create blend state.
        //
        DX::Ensure(device->CreateBlendState(&desc.Blend, m_BlendState.GetAddressOf()));

        //
        // Create depth-stencil state.
        //
        DX::Ensure(device->CreateDepthStencilState(&desc.DepthStencil, m_DepthStencilState.GetAddressOf()));

        //
        // Create rasterizer state.
        //
        DX::Ensure(device->CreateRasterizerState(&desc.Rasterizer, m_RasterizerState.GetAddressOf()));

        //
        // Create input layout.
        //
        DX::Ensure(device->CreateInputLayout(
            desc.InputLayout,
            desc.InputLayoutCount,
            desc.VertexShader.Code.data(),
            desc.VertexShader.Code.size(),
            m_InputLayout.GetAddressOf()
        ));

        //
        // Create Vertex Shader.
        //
        DX::Ensure(device->CreateVertexShader(
            desc.VertexShader.Code.data(),
            desc.VertexShader.Code.size(),
            nullptr,
            m_VertexShader.GetAddressOf()
        ));

        //
        // Create Pixel Shader.
        //
        DX::Ensure(device->CreatePixelShader(
            desc.PixelShader.Code.data(),
            desc.PixelShader.Code.size(),
            nullptr,
            m_PixelShader.GetAddressOf()
        ));

        //
        // Create geometry shader.
        //
        if (!desc.GeometryShader.Code.empty())
        {
            DX::Ensure(device->CreateGeometryShader(
                desc.GeometryShader.Code.data(),
                desc.GeometryShader.Code.size(),
                nullptr,
                m_GeometryShader.GetAddressOf()
            ));
        }

        //
        // Create hull shader.
        //
        if (!desc.HullShader.Code.empty())
        {
            DX::Ensure(device->CreateHullShader(
                desc.HullShader.Code.data(),
                desc.HullShader.Code.size(),
                nullptr,
                m_HullShader.GetAddressOf()
            ));
        }

        //
        // Create domain shader.
        //
        if (!desc.DomainShader.Code.empty())
        {
            DX::Ensure(device->CreateDomainShader(
                desc.DomainShader.Code.data(),
                desc.DomainShader.Code.size(),
                nullptr,
                m_DomainShader.GetAddressOf()
            ));
        }
    }

    GraphicsPipelineState::~GraphicsPipelineState() noexcept
    {
    }
}
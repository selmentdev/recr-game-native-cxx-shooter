#ifndef INCLUDED_CORE_RENDERING_GRAPHICSPIPELINESTATE_HXX
#define INCLUDED_CORE_RENDERING_GRAPHICSPIPELINESTATE_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core.Rendering/Common.hxx>
#include <Core.Rendering/Resource.hxx>

namespace Core::Rendering
{
    struct ShaderDesc final
    {
        std::vector<uint8_t> Code;
    };

    struct GraphicsPipelineStateDesc final
    {
        D3D11_BLEND_DESC Blend;
        D3D11_DEPTH_STENCIL_DESC DepthStencil;
        D3D11_RASTERIZER_DESC Rasterizer;
        D3D11_INPUT_ELEMENT_DESC* InputLayout;
        UINT InputLayoutCount;
        D3D_PRIMITIVE_TOPOLOGY PrimitiveTopology;
        ShaderDesc PixelShader;
        ShaderDesc VertexShader;
        ShaderDesc GeometryShader;
        ShaderDesc HullShader;
        ShaderDesc DomainShader;
    };

    using GraphicsPipelineStateRef = Reference<class GraphicsPipelineState>;
    class GraphicsPipelineState : public Resource
    {
        friend class RenderSystem;
        friend class CommandList;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11BlendState> m_BlendState;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> m_InputLayout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> m_PixelShader;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> m_VertexShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_GeometryShader;
        Microsoft::WRL::ComPtr<ID3D11HullShader> m_HullShader;
        Microsoft::WRL::ComPtr<ID3D11DomainShader> m_DomainShader;
        D3D_PRIMITIVE_TOPOLOGY m_PrimitiveTopology;

    public:
        GraphicsPipelineState(RenderSystem* renderSystem, const GraphicsPipelineStateDesc& desc) noexcept;
        virtual ~GraphicsPipelineState() noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_GRAPHICSPIPELINESTATE_HXX
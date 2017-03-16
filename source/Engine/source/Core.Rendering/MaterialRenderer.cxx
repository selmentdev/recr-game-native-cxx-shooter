//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/MaterialRenderer.hxx>
#include <Core.Rendering/RenderSystem.hxx>
#include <Core/FileSystem.hxx>

namespace Core::Rendering
{

    MaterialRenderer::MaterialRenderer(const std::string& pixelShader, const std::string& vertexShader) noexcept
    {
        //
        // Setup color.
        //
        SetDiffuseColor(DirectX::Colors::DarkGoldenrod);

        //
        // Get render system.
        //
        auto renderSystem = Core::Rendering::RenderSystem::Current;

        //
        // Setup material data uniform buffer.
        //
        {
            Core::Rendering::BufferDesc buffer
            {
                &m_ShaderParams,
                sizeof(m_ShaderParams)
            };

            m_ShaderParamsBuffer = renderSystem->MakeUniformBuffer(buffer);
        }

        //
        // Create pipeline state object. Somewhat Vulkan`ish or D3D12`ish :)
        //
        // Good design though.
        //

        Core::Rendering::GraphicsPipelineStateDesc gd{};

        //
        // Setup render target...
        //
        gd.Blend.RenderTarget[0].BlendEnable = TRUE;
        gd.Blend.RenderTarget[0].SrcBlend = gd.Blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        gd.Blend.RenderTarget[0].DestBlend = gd.Blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        gd.Blend.RenderTarget[0].BlendOp = gd.Blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        gd.Blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        //
        // ...depth stencil...
        //
        gd.DepthStencil.DepthEnable = TRUE;
        gd.DepthStencil.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        gd.DepthStencil.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        gd.DepthStencil.StencilEnable = TRUE;
        gd.DepthStencil.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        gd.DepthStencil.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
        gd.DepthStencil.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        gd.DepthStencil.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        gd.DepthStencil.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        gd.DepthStencil.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        gd.DepthStencil.BackFace = gd.DepthStencil.FrontFace;

        //
        // ...rasterizer...
        //
        gd.Rasterizer.CullMode = D3D11_CULL_BACK;
        gd.Rasterizer.FillMode = D3D11_FILL_SOLID;
        gd.Rasterizer.DepthClipEnable = TRUE;
        gd.Rasterizer.MultisampleEnable = TRUE;

        //
        // Describe vertex format as input layout from MeshRenderer.cxx :)
        //
        D3D11_INPUT_ELEMENT_DESC input[]
        {
            { "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };

        gd.InputLayout = input;
        gd.InputLayoutCount = sizeof(input) / sizeof(input[0]);

        //
        // Default to triangle list.
        //
        gd.PrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        //
        // We support two requred shader types right now :)
        //
        Core::FileSystem::Load(gd.VertexShader.Code, vertexShader);
        Core::FileSystem::Load(gd.PixelShader.Code, pixelShader);

        //
        // Make it run!
        //
        m_PipelineState = renderSystem->MakeGraphicsPipelineState(gd);
    }

    MaterialRenderer::~MaterialRenderer() noexcept
    {
    }

    void MaterialRenderer::Bind(const Core::Rendering::CommandListRef& commandList) noexcept
    {
        //
        // Update buffer content.
        //
        commandList->UpdateUniformBuffer(m_ShaderParamsBuffer, &m_ShaderParams, sizeof(m_ShaderParams));

        //
        // Bind buffer to pixel shader.
        //
        commandList->BindUniformBuffer(Core::Rendering::ShaderMask::Pixel, 0, m_ShaderParamsBuffer);

        //
        // Bind pipeline state.
        //
        commandList->BindGraphicsPipelineState(m_PipelineState);

        //
        // Bind sampler.
        //
        commandList->BindSampler(Core::Rendering::ShaderMask::Pixel, 0, m_TextureSampler);

        //
        // And texture.
        //
        commandList->BindTexture2D(Core::Rendering::ShaderMask::Pixel, 0, m_Texture);
    }
}
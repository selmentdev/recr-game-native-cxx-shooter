//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/MeshRenderer.hxx>
#include <Core.Rendering/RenderSystem.hxx>

namespace Core::Rendering
{
    //
    // This is simple renderer. It uses simple vertex format for everything.
    //
    struct Vertex final
    {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT3 Normal;
        DirectX::XMFLOAT2 TexCoord;
    };

    MeshRenderer::MeshRenderer() noexcept
    {
        auto renderSystem = Core::Rendering::RenderSystem::Current;

        /*
            
            TexCoords were wrong.

            It's 4AM morning.

            I'm drawing this cube.

            p7------------p6
            |\            |\
            | \           | \
            |  \          |  \
            |   p4------------p5
            |   |         |   |
            |   |         |   |
            p3--|---------p2  |
             \  |          \  |
              \ |           \ |
               \|            \|
                p0------------p1

        */

        auto p0 = DirectX::XMFLOAT3(-0.5F, -0.5F,  0.5F);
        auto p1 = DirectX::XMFLOAT3( 0.5F, -0.5F,  0.5F);
        auto p2 = DirectX::XMFLOAT3( 0.5F, -0.5F, -0.5F);
        auto p3 = DirectX::XMFLOAT3(-0.5F, -0.5F, -0.5F);
        auto p4 = DirectX::XMFLOAT3(-0.5F,  0.5F,  0.5F);
        auto p5 = DirectX::XMFLOAT3( 0.5F,  0.5F,  0.5F);
        auto p6 = DirectX::XMFLOAT3( 0.5F,  0.5F, -0.5F);
        auto p7 = DirectX::XMFLOAT3(-0.5F,  0.5F, -0.5F);


        auto nu = DirectX::XMFLOAT3(0.0F, 1.0F, 0.0F);
        auto nd = DirectX::XMFLOAT3(0.0F, -1.0F, 0.0F);
        auto nf = DirectX::XMFLOAT3(0.0F, 0.0F, 1.0F);
        auto nb = DirectX::XMFLOAT3(0.0F, 0.0F, -1.0F);
        auto nl = DirectX::XMFLOAT3(-1.0F, 0.0F, 0.0F);
        auto nr = DirectX::XMFLOAT3(1.0F, 0.0F, 0.0F);


        auto _00 = DirectX::XMFLOAT2(0.0F, 0.0F);
        auto _10 = DirectX::XMFLOAT2(1.0F, 0.0F);
        auto _01 = DirectX::XMFLOAT2(0.0F, 1.0F);
        auto _11 = DirectX::XMFLOAT2(1.0F, 1.0F);

        std::array<Vertex, 24> vertices = { {
            // Bottom
            { p0, nd, _11 }, { p1, nd, _01 }, { p2, nd, _00 }, { p3, nd, _10 },

            // Left
            { p7, nl, _11 }, { p4, nl, _01 }, { p0, nl, _00 }, { p3, nl, _10 },

            // Front
            { p4, nf, _11 }, { p5, nf, _01 }, { p1, nf, _00 }, { p0, nf, _10 },

            // Back
            { p6, nb, _11 }, { p7, nb, _01 }, { p3, nb, _00 }, { p2, nb, _10 },

            // Right
            { p5, nr, _11 }, { p6, nr, _01 }, { p2, nr, _00 }, { p1, nr, _10 },

            // Top
            { p7, nu, _10 }, { p6, nu, _11 }, { p5, nu, _01 }, { p4, nu, _00 },

            } };

        std::array<uint16_t, 3 * 2 * 6> indices = {

            // Bottom
            3 + 4 * 0, 1 + 4 * 0, 0 + 4 * 0,
            3 + 4 * 0, 2 + 4 * 0, 1 + 4 * 0,

            // Left
            3 + 4 * 1, 1 + 4 * 1, 0 + 4 * 1,
            3 + 4 * 1, 2 + 4 * 1, 1 + 4 * 1,

            // Front
            3 + 4 * 2, 1 + 4 * 2, 0 + 4 * 2,
            3 + 4 * 2, 2 + 4 * 2, 1 + 4 * 2,

            // Back
            3 + 4 * 3, 1 + 4 * 3, 0 + 4 * 3,
            3 + 4 * 3, 2 + 4 * 3, 1 + 4 * 3,

            // Right
            3 + 4 * 4, 1 + 4 * 4, 0 + 4 * 4,
            3 + 4 * 4, 2 + 4 * 4, 1 + 4 * 4,

            // Top
            3 + 4 * 5, 1 + 4 * 5, 0 + 4 * 5,
            3 + 4 * 5, 2 + 4 * 5, 1 + 4 * 5,
        };

        //
        // Prepare vertex buffer.
        //
        {
            Core::Rendering::BufferDesc buffer
            {
                reinterpret_cast<void*>(&vertices[0]),
                sizeof(vertices)
            };

            m_VertexBuffer = renderSystem->MakeVertexBuffer(buffer);
        }

        //
        // Prepare index buffer.
        //
        {
            Core::Rendering::BufferDesc buffer
            {
                reinterpret_cast<void*>(&indices[0]),
                sizeof(indices)
            };

            m_IndexBuffer = renderSystem->MakeIndexBuffer(buffer);
        }
    }

    MeshRenderer::~MeshRenderer() noexcept
    {
    }

    void MeshRenderer::Bind(const Rendering::CommandListRef& commandList) noexcept
    {
        //
        // Bind required buffers.
        //
        commandList->BindVertexBuffer(0, m_VertexBuffer, sizeof(Vertex), 0);
        commandList->BindIndexBuffer(m_IndexBuffer, true);

        //
        // This approach is fast, but instancing would be faster.
        //
    }

    void MeshRenderer::Render(const Rendering::CommandListRef& commandList) noexcept
    {
        //
        // Draw them :)
        //
        commandList->DrawIndexed(3 * 2 * 6, 0, 0);
    }
}
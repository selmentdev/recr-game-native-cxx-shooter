//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/Buffers.hxx>
#include <Core.Rendering/RenderSystem.hxx>

namespace Core::Rendering
{
    VertexBuffer::VertexBuffer(RenderSystem* renderSystem, const BufferDesc& desc) noexcept
        : m_RenderSystem{ renderSystem }
        , m_Buffer{}
    {
        auto device = this->m_RenderSystem->m_Device;

        //
        // Standard immutable D3D11 buffer.
        //
        D3D11_BUFFER_DESC sd{};
        sd.Usage = D3D11_USAGE_IMMUTABLE;
        sd.ByteWidth = static_cast<::UINT>(desc.Size);
        sd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        sd.CPUAccessFlags = 0;
        sd.MiscFlags = 0;

        //
        // Setup initial data.
        //
        D3D11_SUBRESOURCE_DATA sr{};
        sr.pSysMem = desc.Pointer;

        //
        // Create buffer.
        //
        DX::Ensure(device->CreateBuffer(&sd, &sr, m_Buffer.GetAddressOf()));
    }

    VertexBuffer::~VertexBuffer() noexcept
    {
    }
}
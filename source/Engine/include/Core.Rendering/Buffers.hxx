#ifndef INCLUDED_CORE_RENDERING_BUFFER_HXX
#define INCLUDED_CORE_RENDERING_BUFFER_HXX

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
    //
    // Note:
    //
    // Currenty, this implementation doesn't support staging buffers for dynamic data upload.
    //

    struct BufferDesc final
    {
    public:
        void* Pointer;
        size_t Size;

    public:
        BufferDesc() = delete;
        BufferDesc(void* pointer, size_t size) noexcept
            : Pointer{ pointer }
            , Size{ size }
        {
        }

        template <typename T>
        BufferDesc(T& data) noexcept
            : BufferDesc(&data, sizeof(data))
        {
        }
    };

    class RenderSystem;

    using VertexBufferRef = Reference<class VertexBuffer>;
    class VertexBuffer : public Resource
    {
        friend class CommandList;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;

    public:
        VertexBuffer(RenderSystem* renderSystem, const BufferDesc& desc) noexcept;
        virtual ~VertexBuffer() noexcept;
    };

    using IndexBufferRef = Reference<class IndexBuffer>;
    class IndexBuffer : public Resource
    {
        friend class CommandList;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;

    public:
        IndexBuffer(RenderSystem* renderSystem, const BufferDesc& desc) noexcept;
        virtual ~IndexBuffer() noexcept;
    };

    using UniformBufferRef = Reference<class UniformBuffer>;
    class UniformBuffer : public Resource
    {
        friend class CommandList;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_Buffer;

    public:
        UniformBuffer(RenderSystem* renderSystem, const BufferDesc& desc) noexcept;
        virtual ~UniformBuffer() noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_BUFFER_HXX
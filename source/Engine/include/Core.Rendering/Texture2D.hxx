#ifndef INCLUDED_CORE_RENDERING_TEXTURE2D_HXX
#define INCLUDED_CORE_RENDERING_TEXTURE2D_HXX

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
    class RenderSystem;

    using Texture2DRef = Reference<class Texture2D>;
    class Texture2D : public Resource
    {
        friend class RenderSystem;
        friend class CommandList;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_Texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;

    public:
        Texture2D(RenderSystem* renderSystem, const std::string& path) noexcept;
        virtual ~Texture2D() noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_TEXTURE2D_HXX
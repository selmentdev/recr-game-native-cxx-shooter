#ifndef INCLUDED_CORE_RENDERING_SAMPLER_HXX
#define INCLUDED_CORE_RENDERING_SAMPLER_HXX

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

    struct SamplerDesc final
    {
        D3D11_SAMPLER_DESC Desc;
    };

    using SamplerRef = Reference<class Sampler>;
    class Sampler : public Resource
    {
        friend class RenderSystem;
        friend class CommandList;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_Sampler;

    public:
        Sampler(RenderSystem* renderSystem, const SamplerDesc& desc) noexcept;
        virtual ~Sampler() noexcept;
    };
}


#endif // INCLUDED_CORE_RENDERING_SAMPLER_HXX
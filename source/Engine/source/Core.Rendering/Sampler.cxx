//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/Sampler.hxx>
#include <Core.Rendering/RenderSystem.hxx>

namespace Core::Rendering
{
    Sampler::Sampler(RenderSystem* renderSystem, const SamplerDesc& desc) noexcept
        : m_RenderSystem{ renderSystem }
        , m_Sampler{}
    {
        //
        // Well, that's just a wrapper. Real engine would support other renderers. Thus this code
        // looks like that. It's just ready to abstract away renderer :)
        //
        auto device = m_RenderSystem->m_Device;
        DX::Ensure(device->CreateSamplerState(&desc.Desc, m_Sampler.GetAddressOf()));
    }

    Sampler::~Sampler() noexcept
    {
    }
}
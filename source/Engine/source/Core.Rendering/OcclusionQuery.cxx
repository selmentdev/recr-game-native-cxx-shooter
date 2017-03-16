//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/Query.hxx>
#include <Core.Rendering/RenderSystem.hxx>

namespace Core::Rendering
{
    OcclusionQuery::OcclusionQuery(RenderSystem* renderSystem) noexcept
        : m_RenderSystem{ renderSystem }
        , m_Query{}
    {
        //
        // Well... Occlusion query. Used for debugging purposes at bootstrap of this project :)
        //
        auto device = m_RenderSystem->m_Device;
        D3D11_QUERY_DESC sd{};
        sd.Query = D3D11_QUERY_OCCLUSION;
        sd.MiscFlags = 0;

        //
        // Create it!
        //
        DX::Ensure(device->CreateQuery(&sd, m_Query.GetAddressOf()));
    }

    OcclusionQuery::~OcclusionQuery() noexcept
    {
    }
}
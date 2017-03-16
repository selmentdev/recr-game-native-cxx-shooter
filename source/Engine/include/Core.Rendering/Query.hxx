#ifndef INCLUDED_CORE_RENDERING_QUERY_HXX
#define INCLUDED_CORE_RENDERING_QUERY_HXX

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

    using OcclusionQueryRef = Reference<class OcclusionQuery>;
    class OcclusionQuery : public Resource
    {
        friend class CommandList;
        friend class RenderSystem;
    private:
        RenderSystem* m_RenderSystem;
        Microsoft::WRL::ComPtr<ID3D11Query> m_Query;

    public:
        OcclusionQuery(RenderSystem* renderSystem) noexcept;
        virtual ~OcclusionQuery() noexcept;
    };
}

#endif // INCLUDED_CORE_RENDERING_QUERY_HXX
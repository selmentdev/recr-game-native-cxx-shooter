#ifndef INCLUDED_CORE_RENDERING_RESOURCE_HXX
#define INCLUDED_CORE_RENDERING_RESOURCE_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core.Rendering/Common.hxx>
#include <Core/Reference.hxx>

namespace Core::Rendering
{
    class Resource : public Object
    {
    public:
        Resource() noexcept;
        virtual ~Resource() noexcept;

    protected:
        Resource(const Resource&) = delete;
        Resource& operator = (const Resource&) = delete;
    };
}

#endif // INCLUDED_CORE_RENDERING_RESOURCE_HXX
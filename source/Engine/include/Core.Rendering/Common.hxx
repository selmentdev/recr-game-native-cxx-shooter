#ifndef INCLUDED_CORE_RENDERING_COMMON_HXX
#define INCLUDED_CORE_RENDERING_COMMON_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <Core.Diagnostics/Trace.hxx>
#include <Core/StringFormat.hxx>

namespace Core::Rendering::DX
{
    __forceinline void Ensure(::HRESULT result)
    {
        if (FAILED(result))
        {
            auto message = StringFormat("HRESULT[%08x] failed at %s:%u", result, __FILE__, __LINE__);

            ::Core::Diagnostics::Trace::WriteLine("%s", message.c_str());
            ::Core::Diagnostics::Debug::Fail(message.c_str());
        }
    }
}

#endif // INCLUDED_CORE_RENDERING_COMMON_HXX
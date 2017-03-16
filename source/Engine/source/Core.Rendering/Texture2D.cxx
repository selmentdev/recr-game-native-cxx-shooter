//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.Rendering/Texture2D.hxx>
#include <Core.Rendering/CommandList.hxx>
#include <Core.Rendering/RenderSystem.hxx>

#include "DDSTextureLoader.h"

namespace Core::Rendering
{
    Texture2D::Texture2D(RenderSystem* renderSystem, const std::string& path) noexcept
        : m_RenderSystem{ renderSystem }
        , m_Texture{ nullptr }
        , m_ShaderResourceView{ nullptr }
    {
        auto device = m_RenderSystem->m_Device;

        //
        // Convert path. Just that.s
        //
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter{};
        auto wpath = converter.from_bytes(path);

        Microsoft::WRL::ComPtr<ID3D11Resource> resource{};

        //
        // Load texture from file.
        //
        // In real engine this is divided into few steps (reading texture from stream,
        // checking if image is valid, allocating proper number of surfaces for texture array and
        // mipmaps...
        //
        // Just delegate it :)
        //
        DX::Ensure(DirectX::CreateDDSTextureFromFile(
            device.Get(),
            wpath.c_str(),
            resource.GetAddressOf(),
            m_ShaderResourceView.GetAddressOf()
        ));

        //
        // **make sure** that this is texture :)
        //
        DX::Ensure(resource.As<ID3D11Texture2D>(&m_Texture));
    }

    Texture2D::~Texture2D() noexcept
    {
    }
}
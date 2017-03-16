//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.World/Camera.hxx>
#include <Core.Rendering/RenderSystem.hxx>

namespace Core::World
{
    Camera::Camera() noexcept
    {

        //
        // Get render system.
        //
        auto renderSystem = Core::Rendering::RenderSystem::Current;

        //
        // Setup material data uniform buffer.
        //
        {
            Core::Rendering::BufferDesc buffer
            {
                &m_ShaderParams,
                sizeof(m_ShaderParams)
            };

            m_ShaderParamsBuffer = renderSystem->MakeUniformBuffer(buffer);
        }
    }

    Camera::~Camera() noexcept
    {
    }

    void XM_CALLCONV Camera::SetLens(float fov, float aspectRatio, float nearZ, float farZ) noexcept
    {
        //
        // Setup projection matrix.
        //
        auto projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);
        DirectX::XMStoreFloat4x4A(&m_ShaderParams.Projection, projection);
    }

    void XM_CALLCONV Camera::LookAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up) noexcept
    {
        //
        // Setup view matrix.
        //
        auto view = DirectX::XMMatrixLookAtLH(position, target, up);
        DirectX::XMStoreFloat4x4A(&m_ShaderParams.View, view);
    }

    void Camera::Bind(const Rendering::CommandListRef& commandList) noexcept
    {
        //
        // Update buffer content.
        //
        commandList->UpdateUniformBuffer(m_ShaderParamsBuffer, &m_ShaderParams, sizeof(m_ShaderParams));

        //
        // Bind buffer to pixel shader.
        //
        commandList->BindUniformBuffer(Core::Rendering::ShaderMask::Vertex, 0, m_ShaderParamsBuffer);
    }
}
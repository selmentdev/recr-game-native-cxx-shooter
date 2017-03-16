#ifndef INCLUDED_CORE_WORLD_CAMERA_HXX
#define INCLUDED_CORE_WORLD_CAMERA_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core/Reference.hxx>
#include <Core.Rendering/Buffers.hxx>
#include <Core.Rendering/CommandList.hxx>

namespace Core::World
{
    using CameraRef = Reference<class Camera>;
    class Camera final : public Object
    {
    public:
        struct ShaderParams
        {
            DirectX::XMFLOAT4X4A View;
            DirectX::XMFLOAT4X4A Projection;
        };

    private:
        ShaderParams m_ShaderParams;
        Core::Rendering::UniformBufferRef m_ShaderParamsBuffer;

    public:
        Camera() noexcept;
        virtual ~Camera() noexcept;

    public:
        void XM_CALLCONV SetLens(float fov, float aspectRatio, float nearZ, float farZ) noexcept;
        void XM_CALLCONV LookAt(DirectX::FXMVECTOR position, DirectX::FXMVECTOR target, DirectX::FXMVECTOR up) noexcept;

    public:
        void Bind(const Rendering::CommandListRef& commandList) noexcept;
    };
}

#endif // INCLUDED_CORE_WORLD_CAMERA_HXX
#ifndef INCLUDED_GAME_LASERBULLET_HXX
#define INCLUDED_GAME_LASERBULLET_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.World/GameObject.hxx>
#include <Core.Rendering/MaterialRenderer.hxx>
#include <Core.Rendering/MeshRenderer.hxx>
#include <Core.World/Physics.hxx>
#include <Core/StringHash.hxx>

namespace GameProject
{
    using namespace Core;

    using LaserBulletRef = Reference<class LaserBullet>;
    class LaserBullet : public World::GameObject
    {
    public:
        static constexpr const World::GameObjectTypeID TypeID = "Game.LaserBullet"_hash32;

    public:
        static constexpr const float TimeToLive = 10.0F;

    private:
        Rendering::MeshRendererRef m_Mesh;
        Rendering::MaterialRendererRef m_Material;
        DirectX::XMFLOAT4X4A m_ScaleMatrix;
        DirectX::XMFLOAT4A m_Direction;
        float m_MoveVelocity;
        float m_LifeTime;

    public:
        static LaserBulletRef Make(DirectX::FXMVECTOR position, DirectX::FXMVECTOR direction, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept;

    public:
        LaserBullet(DirectX::FXMMATRIX transform, DirectX::FXMVECTOR direction, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept;
        virtual ~LaserBullet() noexcept;

        void SetMoveVelocity(float value) noexcept
        {
            m_MoveVelocity = value;
        }

    public:
        virtual void OnUpdate(float deltaTime) noexcept override final;
        virtual void OnRender(const Rendering::CommandListRef& commandList) noexcept override final;
        virtual void OnCollision(GameObject* other) noexcept override final;

    public:
        DirectX::XMMATRIX XM_CALLCONV GetTransform() const noexcept override final;
    };
}

#endif // INCLUDED_GAME_LASERBULLET_HXX
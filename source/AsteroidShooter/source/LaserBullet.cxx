//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <LaserBullet.hxx>

namespace GameProject
{
    LaserBulletRef LaserBullet::Make(DirectX::FXMVECTOR position, DirectX::FXMVECTOR direction, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept
    {
        auto normalizedDirection = DirectX::XMVector3Normalize(direction);
        auto transform = DirectX::XMMatrixTranslationFromVector(position);
        auto result = MakeRef<LaserBullet>(transform, direction, mesh, material);
        return result;
    }

    LaserBullet::LaserBullet(DirectX::FXMMATRIX transform, DirectX::FXMVECTOR direction, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept
        : GameObject(LaserBullet::TypeID)
        , m_Mesh{ mesh }
        , m_Material{ material }
        , m_ScaleMatrix{}
        , m_Direction{}
        , m_MoveVelocity{ 16.0F }
        , m_LifeTime{ 0.0F }
    {
        //
        // Setups rigid body.
        //
        const auto scale = DirectX::XMVectorSet(0.2F, 0.2F, 1.0F, 0.0F);

        const auto scaleMatrix = DirectX::XMMatrixScalingFromVector(scale);

        DirectX::XMStoreFloat4A(&m_Direction, direction);
        DirectX::XMStoreFloat4x4A(&m_ScaleMatrix, scaleMatrix);

        m_RigidBody = World::Physics::MakeRigidBody();
        m_RigidBody->userData = reinterpret_cast<void*>(this);
        m_RigidBody->setGlobalPose(World::Converters::XMMATRIXToPxTransform(transform));
        m_RigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
        m_RigidBody->setRigidDynamicFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);

        auto shape = World::Physics::MakeBoxCollider(DirectX::XMVectorScale(scale, 0.5F));
        m_RigidBody->attachShape(*shape);
    }

    LaserBullet::~LaserBullet() noexcept
    {
    }

    void LaserBullet::OnUpdate(float deltaTime) noexcept
    {
        m_LifeTime += deltaTime;

        //
        // Check if meteorite passed it lifetime.
        //
        if (m_LifeTime > LaserBullet::TimeToLive)
        {
            Destroy();
        }

        //
        // Setup forward linear velocity.
        //
        m_RigidBody->setLinearVelocity(physx::PxVec3{ 0.0F, 0.0F, m_MoveVelocity });
    }

    void LaserBullet::OnRender(const Rendering::CommandListRef& commandList) noexcept
    {
        //
        // Bind material.
        //
        m_Material->Bind(commandList);

        //
        // And render mesh.
        //
        m_Mesh->Bind(commandList);
        m_Mesh->Render(commandList);
    }

    void LaserBullet::OnCollision(GameObject* other) noexcept
    {
        (void)other;

        //
        // Bullet destroys itself.
        //
        Destroy();
    }

    DirectX::XMMATRIX XM_CALLCONV LaserBullet::GetTransform() const noexcept
    {
        //
        // Prepend scaling matrix to result transform.
        //
        auto scaling = DirectX::XMLoadFloat4x4A(&m_ScaleMatrix);

        return DirectX::XMMatrixMultiply(scaling, GameObject::GetTransform());
    }
}
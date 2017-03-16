//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <SpaceShip.hxx>
#include <LaserBullet.hxx>
#include <Game.hxx>

namespace GameProject
{
    using namespace Core;

    SpaceShip::SpaceShip(World::Scene* scene, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material, const Rendering::MeshRendererRef& bulletMesh, const Rendering::MaterialRendererRef& bulletMaterial) noexcept
        : GameObject(SpaceShip::TypeID)
        , m_Mesh{ mesh }
        , m_Material{ material }
        , m_BulletMesh{ bulletMesh }
        , m_BulletMaterial{ bulletMaterial }
        , m_Scene{ scene }
        , m_MoveVelocity{ 0.0F }
        , m_FireTimeout{ 0.0F }
        , m_CannonFlipFactor{ 1.0F }
    {
        //
        // Create and setup rigid body.
        //
        m_RigidBody = World::Physics::MakeRigidBody();
        m_RigidBody->userData = reinterpret_cast<void*>(this);
        m_RigidBody->setGlobalPose(physx::PxTransform::createIdentity());
        m_RigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
        m_RigidBody->setRigidDynamicFlag(physx::PxRigidBodyFlag::eKINEMATIC, true);
        m_RigidBody->setMass(10.0F);

        //
        // And attach box shape.
        //
        auto shape = World::Physics::MakeBoxCollider(DirectX::XMVectorSet(0.5F, 0.5F, 0.5F, 0.0F));
        m_RigidBody->attachShape(*shape);
    }

    SpaceShip::~SpaceShip() noexcept
    {
    }

    void SpaceShip::Fire() noexcept
    {
        //
        // Spawn bullets.
        //
        if (m_FireTimeout >= SpaceShip::FireInterval)
        {
            m_FireTimeout = 0.0F;

            m_CannonFlipFactor *= -1.0F;

            const auto position = World::Converters::PxVec3ToXMVECTOR(m_RigidBody->getGlobalPose().p);
            const auto direction = DirectX::XMVectorSet(m_CannonFlipFactor * 0.25F, 0.0F, 1.0F, 0.0F);
            const auto bulletPosition = DirectX::XMVectorAdd(position, DirectX::XMVectorScale(direction, 2.0F));

            auto bullet = LaserBullet::Make(bulletPosition, direction, m_BulletMesh, m_BulletMaterial);
            m_Scene->Add(bullet);
        }
    }

    void SpaceShip::OnUpdate(float deltaTime) noexcept
    {
        m_FireTimeout += deltaTime;

        //
        // This is kinematic body, so we are responsible for moving it.
        //

        const auto displacement = DirectX::XMVectorSet(m_MoveVelocity * deltaTime, 0.0F, 0.0F, 0.0F);

        physx::PxVec3 pxDisplacement;
        DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&pxDisplacement), displacement);

        physx::PxTransform transform = m_RigidBody->getGlobalPose();
        transform.p += pxDisplacement;

        //
        // Clamp to visible range.
        //
        transform.p.x = Core::Clamp(transform.p.x, -Game::VisibleRangeExtent, Game::VisibleRangeExtent);

        m_RigidBody->setKinematicTarget(transform);
    }

    void SpaceShip::OnRender(const Rendering::CommandListRef& commandList) noexcept
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

    void SpaceShip::OnCollision(GameObject* other) noexcept
    {
        //
        // When spaceship is destroyed, it resets game.
        //
        other->Destroy();
        Destroy();
        Game::Current->Restart();
    }
}
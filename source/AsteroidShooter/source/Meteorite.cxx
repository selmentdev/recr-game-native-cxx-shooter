//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Meteorite.hxx>
#include <LaserBullet.hxx>
#include <Game.hxx>
#include <extensions/PxD6Joint.h>

namespace GameProject
{
    using namespace Core;

    MeteoriteRef XM_CALLCONV Meteorite::Make(DirectX::FXMVECTOR position, DirectX::FXMVECTOR orientation, DirectX::FXMVECTOR velocity, DirectX::GXMVECTOR size, DirectX::HXMVECTOR angularVelocity, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept
    {
        auto meteorite = MakeRef<Meteorite>(position, orientation, velocity, size, angularVelocity, mesh, material);
        return meteorite;
    }

    Meteorite::Meteorite(DirectX::FXMVECTOR position, DirectX::FXMVECTOR orientation, DirectX::FXMVECTOR velocity, DirectX::GXMVECTOR size, DirectX::HXMVECTOR angularVelocity, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept
        : GameObject(Meteorite::TypeID)
        , m_Mesh{ mesh }
        , m_Material{ material }
        , m_DirectionForce{}
        , m_ScaleMatrix{}
        , m_LifeTime{ 0.0F }
    {
        auto scalingMatrix = DirectX::XMMatrixScalingFromVector(size);
        DirectX::XMStoreFloat4x4A(&m_ScaleMatrix, scalingMatrix);

        auto transform = DirectX::XMMatrixAffineTransformation(
            DirectX::XMVectorSet(1.0F, 1.0f, 1.0F, 0.0F),
            DirectX::XMVectorZero(),
            orientation,
            position
        );

        //
        // Setups rigid body.
        //
        m_RigidBody = World::Physics::MakeRigidBody();
        m_RigidBody->userData = reinterpret_cast<void*>(this);
        m_RigidBody->setGlobalPose(physx::PxTransform{ World::Converters::XMMATRIXToPxMatrix(transform) });
        m_RigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
        m_RigidBody->setRigidDynamicFlag(physx::PxRigidBodyFlag::eKINEMATIC, false);
        m_RigidBody->setLinearVelocity(World::Converters::XMVECTORToPxVec3(velocity));
        m_RigidBody->setAngularVelocity(World::Converters::XMVECTORToPxVec3(angularVelocity));
        m_RigidBody->setAngularDamping(0.0F);
        m_RigidBody->setLinearDamping(0.0F);

        auto shape = World::Physics::MakeBoxCollider(DirectX::XMVectorScale(size, 0.5F));
        m_RigidBody->attachShape(*shape);

        //
        // And D6 joint.
        //
        auto dof = physx::PxD6JointCreate(
            *World::Physics::GetPhysx(),
            m_RigidBody,
            physx::PxTransform::createIdentity(),
            nullptr,
            m_RigidBody->getGlobalPose()
        );

        dof->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
        dof->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
        dof->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);

        dof->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eFREE);
        dof->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eFREE);
        dof->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eFREE);

        DirectX::XMStoreFloat4A(&m_DirectionForce, velocity);
    }

    Meteorite::~Meteorite() noexcept
    {
    }

    void Meteorite::OnUpdate(float deltaTime) noexcept
    {
        m_LifeTime += deltaTime;

        //
        // Check if meteorite passed it lifetime.
        //
        if (m_LifeTime > Meteorite::TimeToLive)
        {
            Destroy();
        }

        //
        // This is a little bit hacky. Unity 3D probably does exactly the same thing with freeze
        // axis on rigid bodies.
        //

        //
        // Get current velocty.
        //
        const auto current = World::Converters::PxVec3ToXMVECTOR(m_RigidBody->getLinearVelocity());

        //
        // Get scalar velocity value.
        //
        const auto scalarVelocity = DirectX::XMVector3Length(current);

        //
        // Select x, 0, z, 0 vector
        //
        const auto direction2d = DirectX::XMVectorPermute<0, 4 + 1, 2, 4 + 3>(current, DirectX::XMVectorZero());

        //
        // Normalize direction.
        //
        const auto normalized = DirectX::XMVector3Normalize(direction2d);

        //
        // Compute final velocity vector.
        //
        const auto velocity = DirectX::XMVectorMultiply(normalized, scalarVelocity);

        //
        // And set this velocity as new.
        //
        m_RigidBody->setLinearVelocity(World::Converters::XMVECTORToPxVec3(velocity));
    }

    void Meteorite::OnRender(const Rendering::CommandListRef& commandList) noexcept
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

    void Meteorite::OnCollision(GameObject* other) noexcept
    {
        if (other->TypeID == LaserBullet::TypeID)
        {
            //
            // Notify game that meteorites was shot down.
            //
            Game::Current->NotifyMeteoriteShotDown();

            //
            // Meteorite destroys itself on collision with laser bullet.
            //
            Destroy();
        }
    }

    DirectX::XMMATRIX XM_CALLCONV Meteorite::GetTransform() const noexcept
    {
        //
        // Prepend scaling matrix to result transform.
        //
        auto scaling = DirectX::XMLoadFloat4x4A(&m_ScaleMatrix);

        return DirectX::XMMatrixMultiply(scaling, GameObject::GetTransform());
    }
}
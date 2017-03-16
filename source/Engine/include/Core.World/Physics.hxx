#ifndef INCLUDED_CORE_WORLD_PHYSICS_HXX
#define INCLUDED_CORE_WORLD_PHYSICS_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>

#include <Core.World/Scene.hxx>
#include <foundation/PxVec3.h>
#include <foundation/PxMat44.h>
#include <PxPhysics.h>

namespace Core::World
{
    class Physics final
    {
        friend class RigidBody;
    public:
        Physics() = delete;
        Physics(const Physics&) = delete;
        Physics& operator = (const Physics&) = delete;

    public:
        static void Initialize() noexcept;
        static void Shutdown() noexcept;

    public:
        static Core::World::SceneRef MakeScene(const DirectX::XMFLOAT3& gravity) noexcept;

    public:
        static physx::PxRigidDynamic* Physics::MakeRigidBody() noexcept;

    public:
        static physx::PxShape* MakeBoxCollider(DirectX::FXMVECTOR extent) noexcept;
        static physx::PxShape* MakeSphereCollider(float radius) noexcept;

    public:
        static physx::PxMaterial* GetDefaultMaterial() noexcept;
        static physx::PxPhysics* GetPhysx() noexcept;
    };

    namespace Converters
    {
        static_assert(sizeof(DirectX::XMFLOAT4X4) == sizeof(physx::PxMat44), "I really do miss C++17 static assert without message...");
        static_assert(alignof(DirectX::XMFLOAT4X4) <= alignof(physx::PxMat44), "I really do miss C++17 static assert without message...");

        static_assert(sizeof(DirectX::XMFLOAT3) == sizeof(physx::PxVec3), "I really do miss C++17 static assert without message...");
        static_assert(alignof(DirectX::XMFLOAT3) <= alignof(physx::PxVec3), "I really do miss C++17 static assert without message...");


        __forceinline physx::PxMat44 XM_CALLCONV XMMATRIXToPxMatrix(DirectX::FXMMATRIX m) noexcept
        {
            physx::PxMat44 result;

            DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result), m);
            return result;
        }

        __forceinline DirectX::XMMATRIX XM_CALLCONV PxMatrixToXMMATRIX(const physx::PxMat44& v) noexcept
        {
            return DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&v));
        }

        __forceinline physx::PxVec3 XM_CALLCONV XMVECTORToPxVec3(DirectX::FXMVECTOR v) noexcept
        {
            physx::PxVec3 result;
            DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&result), v);
            return result;
        }

        __forceinline DirectX::XMVECTOR XM_CALLCONV PxVec3ToXMVECTOR(const physx::PxVec3& v) noexcept
        {
            return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&v));
        }

        __forceinline physx::PxTransform XM_CALLCONV XMMATRIXToPxTransform(DirectX::FXMMATRIX t) noexcept
        {
            physx::PxMat44 px;
            DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&px), t);

            physx::PxTransform tx{ px };
            return tx;
        }

        __forceinline DirectX::XMMATRIX XM_CALLCONV PxTransformToXMMATRIX(const physx::PxTransform& t) noexcept
        {
            auto transform = physx::PxMat44{ t };
            return DirectX::XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&transform));
        }
    }
}

#endif // INCLUDED_CORE_WORLD_PHYSICS_HXX
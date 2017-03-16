//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core.World/Physics.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <Core.Diagnostics/Trace.hxx>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <physxprofilesdk/PxProfileZoneManager.h>
#include <extensions/PxDefaultSimulationFilterShader.h>

namespace Core::World
{
    //
    // There may be only one instance of physx library.
    //
    namespace
    {
        physx::PxDefaultAllocator g_PxDefaultAllocator{};
        physx::PxDefaultErrorCallback g_PxDefaultErrorCallback{};

        physx::PxFoundation* g_PxFoundation{};
        physx::PxPhysics* g_PxPhysics{};
        physx::PxProfileZoneManager* g_PxProfileZoneManager{};
        physx::PxDefaultCpuDispatcher* g_PxDefaultCpuDispatcher{};
        physx::PxTolerancesScale g_PxToleranceScale{};

        physx::PxMaterial* g_PxDefaultMaterial{};

        static physx::PxFilterFlags physicsFilterShader(
            physx::PxFilterObjectAttributes attributes0,
            physx::PxFilterData filterData0,
            physx::PxFilterObjectAttributes attributes1,
            physx::PxFilterData filterData1,
            physx::PxPairFlags& pairFlags,
            const void* constantBlock,
            physx::PxU32 constantBlockSize)
        {
            (void)attributes0;
            (void)filterData0;
            (void)attributes1;
            (void)filterData1;
            (void)constantBlock;
            (void)constantBlockSize;

            //
            // Want resolve contacts and notify about collisions.
            //
            pairFlags = physx::PxPairFlag::eRESOLVE_CONTACTS;
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;
            return physx::PxFilterFlags();
        }
    }

    void Physics::Initialize() noexcept
    {
        //
        // Standard physx initialization :)
        //
        g_PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_PxDefaultAllocator, g_PxDefaultErrorCallback);
        CORE_ASSERT(g_PxFoundation != nullptr);
        CORE_TRACE_MESSAGE(Info, "[PhysX] Initialized PxFoundation");

        g_PxDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
        CORE_ASSERT(g_PxDefaultCpuDispatcher != nullptr);
        CORE_TRACE_MESSAGE(Info, "[PhysX] Allocated PxDefaultCpuDispatcher");

        g_PxProfileZoneManager = &physx::PxProfileZoneManager::createProfileZoneManager(g_PxFoundation);
        CORE_ASSERT(g_PxProfileZoneManager != nullptr);
        CORE_TRACE_MESSAGE(Info, "[PhysX] Allocated PxProfileZoneManager");

        g_PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PxFoundation, g_PxToleranceScale, false, g_PxProfileZoneManager);
        CORE_ASSERT(g_PxPhysics != nullptr);
        CORE_TRACE_MESSAGE(Info, "[PhysX] Allocated PxPhysics");

        g_PxDefaultMaterial = g_PxPhysics->createMaterial(0.5F, 0.5F, 0.1F);
        CORE_ASSERT(g_PxDefaultMaterial != nullptr);
        CORE_TRACE_MESSAGE(Info, "[PhysX] Allocated PxDefaultMaterial");
    }

    void Physics::Shutdown() noexcept
    {
        //
        // And standard physx shutdown
        //
        g_PxDefaultMaterial->release();
        CORE_TRACE_MESSAGE(Info, "[PhysX] Deallocate PxDefaultMaterial");

        g_PxPhysics->release();
        CORE_TRACE_MESSAGE(Info, "[PhysX] Deallocate PxPhysics");

        g_PxProfileZoneManager->release();
        CORE_TRACE_MESSAGE(Info, "[PhysX] Deallocate PxProfileZoneManager");

        g_PxDefaultCpuDispatcher->release();
        CORE_TRACE_MESSAGE(Info, "[PhysX] Deallocate PxDefaultCpuDispatcher");

        g_PxFoundation->release();
        CORE_TRACE_MESSAGE(Info, "[PhysX] Shutdown PxFoundation");
    }

    Core::World::SceneRef Physics::MakeScene(const DirectX::XMFLOAT3& gravity) noexcept
    {
        CORE_TRACE_MESSAGE(Info, "[PhysX] Create new scene");

        physx::PxSceneDesc desc{ g_PxToleranceScale };

        //
        // Default scene params.
        //
        desc.gravity = physx::PxVec3(gravity.x, gravity.y, gravity.z);
        desc.cpuDispatcher = g_PxDefaultCpuDispatcher;
        desc.flags |= physx::PxSceneFlag::eENABLE_ACTIVETRANSFORMS;
        desc.filterShader = physicsFilterShader;

        //
        // Create actual scene.
        //
        return MakeRef<Scene>(g_PxPhysics, desc);
    }

    physx::PxRigidDynamic* Physics::MakeRigidBody() noexcept
    {
        //
        // Create empty dynamic rigid body.
        //
        auto actor = g_PxPhysics->createRigidDynamic(physx::PxTransform{ physx::PxIdentity });
        return actor;
    }


    physx::PxShape* Physics::MakeBoxCollider(DirectX::FXMVECTOR extent) noexcept
    {
        //
        // Create simple box collider shape.
        //
        DirectX::XMFLOAT3A values;
        DirectX::XMStoreFloat3A(&values, extent);

        auto shape = g_PxPhysics->createShape(physx::PxBoxGeometry(values.x, values.y, values.z), *g_PxDefaultMaterial);
        return shape;
    }

    physx::PxShape* Physics::MakeSphereCollider(float radius) noexcept
    {
        //
        // Create sphere collider.
        //
        auto shape = g_PxPhysics->createShape(physx::PxSphereGeometry(radius), *g_PxDefaultMaterial);
        return shape;
    }

    physx::PxMaterial* Physics::GetDefaultMaterial() noexcept
    {
        return g_PxDefaultMaterial;
    }

    physx::PxPhysics* Physics::GetPhysx() noexcept
    {
        return g_PxPhysics;
    }
}
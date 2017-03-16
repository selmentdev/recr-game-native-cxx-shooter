#ifndef INCLUDED_CORE_WORLD_SCENE_HXX
#define INCLUDED_CORE_WORLD_SCENE_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/Common.hxx>
#include <Core/Reference.hxx>
#include <Core.World/GameObject.hxx>
#include <Core.World/Camera.hxx>

#include <PxPhysics.h>
#include <PxScene.h>
#include <PxSimulationEventCallback.h>

namespace Core::World
{
    using SceneRef = Reference<class Scene>;
    class Scene : public Object, private physx::PxSimulationEventCallback
    {
        friend class Physics;

    private:
        static constexpr const float FixedDeltaTime = 0.02F;

    private:
        //
        // Scene params for GPU.
        //
        // Note:
        //      Currently 32bit build is not supported.
        //
        //      It might be when **someone** will implement aligned new / delete for certain classes
        //      **OR** we get C++17 aligned new in MSVS :)
        //
        //      http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0035r4.html
        //
        struct SceneParams
        {
            DirectX::XMFLOAT4X4A World;
            DirectX::XMFLOAT4X4A InverseWorld;
        };
        static_assert(alignof(SceneParams) >= alignof(DirectX::XMVECTOR), "");

    private:
        physx::PxScene* m_Scene;
        physx::PxPhysics* m_Physics;

        std::vector<GameObjectRef> m_Objects;

        Core::World::CameraRef m_Camera;
        Core::Rendering::UniformBufferRef m_CurrentObject;

        SceneParams m_SceneParams;

    public:
        Scene(physx::PxPhysics* physics, physx::PxSceneDesc scene) noexcept;
        virtual ~Scene() noexcept;

    public:
        Core::World::CameraRef GetCamera() const noexcept
        {
            return m_Camera;
        }

        size_t GetObjectsCount() const noexcept
        {
            return m_Objects.size();
        }

        void Clear() noexcept;

    private:
        void RenderSingleObject(const World::GameObjectRef& gameObject, const Rendering::CommandListRef& commandList) noexcept;

    public:
        void OnUpdate(float deltaTime) noexcept;
        void OnRender(const Rendering::CommandListRef& commandList) noexcept;

    public:
        void Add(const GameObjectRef& gameObject) noexcept;
        void Remove(const GameObjectRef& gameObject) noexcept;
        void Tick(float deltaTime) noexcept;

    private:
        void RemovePendingObjects() noexcept;

    private:
        virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override final;
        virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override final;
        virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override final;
        virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override final;
        virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override final;
    };
}

#endif // INCLUDED_CORE_WORLD_SCENE_HXX

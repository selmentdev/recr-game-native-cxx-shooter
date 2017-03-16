#ifndef INCLUDED_GAME_SPACESHIP_HXX
#define INCLUDED_GAME_SPACESHIP_HXX

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

    using SpaceShipRef = Reference<class SpaceShip>;
    class SpaceShip : public World::GameObject
    {
    public:
        static constexpr const World::GameObjectTypeID TypeID = "Game.SpaceShip"_hash32;
        static constexpr const float FireInterval = 0.25F;

    private:
        Rendering::MeshRendererRef m_Mesh;
        Rendering::MaterialRendererRef m_Material;
        Rendering::MeshRendererRef m_BulletMesh;
        Rendering::MaterialRendererRef m_BulletMaterial;
        World::Scene* m_Scene;
        float m_MoveVelocity;
        float m_FireTimeout;
        float m_CannonFlipFactor;

    public:
        SpaceShip(World::Scene* scene, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material, const Rendering::MeshRendererRef& bulletMesh, const Rendering::MaterialRendererRef& bulletMaterial) noexcept;
        virtual ~SpaceShip() noexcept;

    public:
        void SetHorizontalVelocity(float value) noexcept
        {
            m_MoveVelocity = value;
        }

        void Fire() noexcept;

    public:
        virtual void OnUpdate(float deltaTime) noexcept override final;
        virtual void OnRender(const Rendering::CommandListRef& commandList) noexcept override final;
        virtual void OnCollision(GameObject* other) noexcept override final;
    };

}

#endif // INCLUDED_GAME_SPACESHIP_HXX
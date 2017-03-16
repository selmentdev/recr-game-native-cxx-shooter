#ifndef INCLUDED_GAME_METEORITE_HXX
#define INCLUDED_GAME_METEORITE_HXX

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

    using MeteoriteRef = Reference<class Meteorite>;
    class Meteorite : public World::GameObject
    {
    public:
        static constexpr const World::GameObjectTypeID TypeID = "Game.Meteorite"_hash32;
        static constexpr const float TimeToLive = 6.0F; // Fair enough
    private:
        Rendering::MeshRendererRef m_Mesh;
        Rendering::MaterialRendererRef m_Material;
        DirectX::XMFLOAT4A m_DirectionForce;
        DirectX::XMFLOAT4X4A m_ScaleMatrix;
        float m_LifeTime;

    public:
        //
        // Fancy thing: static constructor.
        //
        // A little bit longer definition, but possible to add it to some kind of factory for serialization purposes - GameObjects have TypeID already :)
        //
        //
        static MeteoriteRef XM_CALLCONV Make(DirectX::FXMVECTOR position, DirectX::FXMVECTOR orientation, DirectX::FXMVECTOR velocity, DirectX::GXMVECTOR size, DirectX::HXMVECTOR angularVelocity, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept;

    public:
        Meteorite(DirectX::FXMVECTOR position, DirectX::FXMVECTOR orientation, DirectX::FXMVECTOR velocity, DirectX::GXMVECTOR size, DirectX::HXMVECTOR angularVelocity, const Rendering::MeshRendererRef& mesh, const Rendering::MaterialRendererRef& material) noexcept;
        virtual ~Meteorite() noexcept;

    public:
        virtual void OnUpdate(float deltaTime) noexcept override final;
        virtual void OnRender(const Rendering::CommandListRef& commandList) noexcept override final;
        virtual void OnCollision(GameObject* other) noexcept override final;

    public:
        virtual DirectX::XMMATRIX XM_CALLCONV GetTransform() const noexcept override final;
    };
}

#endif // INCLUDED_GAME_METEORITE_HXX
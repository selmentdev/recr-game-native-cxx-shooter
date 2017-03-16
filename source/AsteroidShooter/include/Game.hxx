#ifndef INCLUDED_GAME_HXX
#define INCLUDED_GAME_HXX

//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Core/CoreEventHandler.hxx>
#include <Core/CoreWindow.hxx>
#include <Core.World/Scene.hxx>
#include <Core.World/Physics.hxx>
#include <Core.Rendering/CommandList.hxx>
#include <Core.Rendering/RenderSystem.hxx>

#include <Core.Rendering/MaterialRenderer.hxx>
#include <Core.Rendering/MeshRenderer.hxx>
#include <Core.World/Camera.hxx>

#include <SpaceShip.hxx>
#include <Meteorite.hxx>

#include <random>

namespace GameProject
{
    using namespace Core;

    class Game : public CoreEventHandler
    {
    public:
        static Game* Current;

    private:
        CoreWindowRef m_Window;
        Rendering::ViewportRef m_Viewport;

        World::SceneRef m_Scene;
        Rendering::OcclusionQueryRef m_Query;

        Rendering::MaterialRendererRef m_SpaceShipMaterial;
        Rendering::MeshRendererRef m_SpaceShipMesh;

        Rendering::MaterialRendererRef m_BulletMaterial;
        Rendering::MeshRendererRef m_BulletMesh;

        Rendering::MaterialRendererRef m_MeteoriteMaterial;
        Rendering::MeshRendererRef m_MeteoriteMesh;

        SpaceShipRef m_SpaceShip;

        float m_MoveLeftVelocity;
        float m_MoveRightVelocity;

        bool m_FireDown;
        bool m_IsPaused;

    public:
        static constexpr const auto VisibleRangeExtent = 20.0F;

    private:
        std::mt19937 m_RandomEngine;
        float m_SpawnTimeout;
        float m_SpawnInterval;
        uint32_t m_FrameCount;
        float m_FrameCounterTimeout;
        bool m_IsRestarting;
        uint32_t m_MeteoritesShotDown;

    public:
        Game() noexcept;
        virtual ~Game() noexcept;

    public:
        virtual void OnWindowClose(CoreWindow* window) noexcept override final;
        virtual bool OnKeyDown(uint32_t keyCode, char32_t character, bool isRepeat) noexcept override final;
        virtual bool OnKeyUp(uint32_t keyCode, char32_t character, bool isRepeat) noexcept override final;

        virtual bool OnWindowSizeChanged(CoreWindow* window, int32_t width, int32_t height, bool wasMinimized) noexcept override final;
        virtual bool OnWindowActivated(CoreWindow* window, CoreWindowActivation activationType) noexcept override final;
        virtual bool OnApplicationActivated(bool isActive) noexcept override final;
        virtual bool OnWindowEnterSizeMove(CoreWindow* window) noexcept override final;
        virtual void OnWindowExitSizeMove(CoreWindow* window) noexcept override final;

        virtual void Tick(float deltaTime) noexcept;
        virtual void Render(float deltaTime) noexcept;

    public:
        void Restart() noexcept;
        void NotifyMeteoriteShotDown() noexcept;

    private:
        void DoRestart() noexcept;
        void RecomputeInterval() noexcept;

    public:
        virtual void Initialize() noexcept;
        virtual void Shutdown() noexcept;

    private:
        DirectX::XMVECTOR XM_CALLCONV RandomVector2D() noexcept;
        DirectX::XMVECTOR XM_CALLCONV RandomVector3D() noexcept;
        DirectX::XMVECTOR XM_CALLCONV RandomVector3D(DirectX::FXMVECTOR min, DirectX::FXMVECTOR max) noexcept;
        DirectX::XMVECTOR XM_CALLCONV RandomUnitVector() noexcept;
        DirectX::XMVECTOR XM_CALLCONV RandomQuaternion() noexcept;
        DirectX::XMVECTOR XM_CALLCONV RandomAngularVelocity(float max) noexcept;

        float RandomScalar(float min, float max) noexcept;

        void SpawnMeteorite() noexcept;
    };
}
#endif // INCLUDED_GAME_HXX
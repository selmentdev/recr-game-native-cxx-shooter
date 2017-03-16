//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Game.hxx>
#include <Core/Environment.hxx>
#include <Core/FileSystem.hxx>
#include <Core/CoreApplication.hxx>
#include <Core/StringFormat.hxx>

namespace GameProject
{
    using namespace Core;

    Game* Game::Current{ nullptr };

    Game::Game() noexcept
        : m_Scene{}
        , m_Query{}
        , m_SpaceShipMaterial{}
        , m_SpaceShipMesh{}
        , m_BulletMaterial{}
        , m_BulletMesh{}
        , m_MeteoriteMaterial{}
        , m_MeteoriteMesh{}
        , m_SpaceShip{}
        , m_MoveLeftVelocity{ 0.0F }
        , m_MoveRightVelocity{ 0.0F }
        , m_RandomEngine{}
        , m_SpawnTimeout{}
        , m_SpawnInterval{}
        , m_FrameCount{ 0 }
        , m_FrameCounterTimeout{ 0.0F }
        , m_FireDown{ false }
        , m_IsRestarting{ false }
        , m_MeteoritesShotDown{ 0 }
    {
        CORE_ASSERT(Game::Current == nullptr);
        Game::Current = this;

        ::ShowCursor(FALSE);
    }

    Game::~Game() noexcept
    {
        CORE_ASSERT(Game::Current == this);
        Game::Current = nullptr;
    }

    void Game::OnWindowClose(CoreWindow* window) noexcept
    {
        (void)window;
        Environment::RequestExit();
    }


    bool Game::OnKeyDown(uint32_t keyCode, char32_t character, bool isRepeat) noexcept
    {
        (void)isRepeat;

        if (character == 'a' || character == 'A' || keyCode == VK_LEFT)
        {
            m_MoveLeftVelocity = -10.0F;
        }
        else if (character == 'd' || character == 'D' || keyCode == VK_RIGHT)
        {
            m_MoveRightVelocity = 10.0F;
        }
        else if (keyCode == VK_SPACE)
        {
            m_FireDown = true;
        }

        return true;
    }

    bool Game::OnKeyUp(uint32_t keyCode, char32_t character, bool isRepeat) noexcept
    {
        (void)isRepeat;

        if (character == 'a' || character == 'A' || keyCode == VK_LEFT)
        {
            m_MoveLeftVelocity = 0.0F;
        }
        else if (character == 'd' || character == 'D' || keyCode == VK_RIGHT)
        {
            m_MoveRightVelocity = 0.0F;
        }
        else if (keyCode == VK_SPACE)
        {
            m_FireDown = false;
        }

        return true;
    }

    bool Game::OnWindowSizeChanged(CoreWindow* window, int32_t width, int32_t height, bool wasMinimized) noexcept
    {
        (void)window;
        (void)wasMinimized;

        if (m_Viewport != nullptr)
        {
            Rendering::RenderSystem::Current->ResizeViewport(m_Viewport, width, height, false);
        }

        return true;
    }

    bool Game::OnWindowActivated(CoreWindow* window, CoreWindowActivation activationType) noexcept
    {
        (void)window;
        m_IsPaused = (activationType == CoreWindowActivation::Deactivate);
        return true;
    }

    bool Game::OnApplicationActivated(bool isActive) noexcept
    {
        m_IsPaused = !isActive;
        return true;
    }

    bool Game::OnWindowEnterSizeMove(CoreWindow* window) noexcept
    {
        (void)window;
        m_IsPaused = true;
        return true;
    }

    void Game::OnWindowExitSizeMove(CoreWindow* window) noexcept
    {
        (void)window;
        m_IsPaused = false;
    }

    void Game::Tick(float deltaTime) noexcept
    {
        if (m_IsPaused)
        {
            //
            // Game is paused. Don't tick gameplaya.
            //
            return;
        }

        ++m_FrameCount;
        m_FrameCounterTimeout += deltaTime;

        if (m_FrameCounterTimeout >= 3.0F)
        {
            float framesPerSecond = static_cast<float>(m_FrameCount) / m_FrameCounterTimeout;

            DirectX::XMFLOAT3A shipPosition;
            DirectX::XMStoreFloat3A(&shipPosition, m_SpaceShip->GetPosition());

            auto text = StringFormat("Tick: %f, FPS: %f, ObjCount: %zu, ShotDown: %" PRIu32 ", SpawnInterval: %f, ShipXPos: %f",
                deltaTime,
                framesPerSecond,
                m_Scene->GetObjectsCount(),
                m_MeteoritesShotDown,
                m_SpawnInterval,
                shipPosition.x
            );

            m_Window->SetText(text.c_str());

            m_FrameCounterTimeout = 0.0F;
            m_FrameCount = 0;
        }

        //
        // Spawn meteorites.
        //
        m_SpawnTimeout += deltaTime;

        while (m_SpawnTimeout >= m_SpawnInterval)
        {
            m_SpawnTimeout -= m_SpawnInterval;
            SpawnMeteorite();
        }

        //
        // Set spaceship motion.
        //
        m_SpaceShip->SetHorizontalVelocity(m_MoveLeftVelocity + m_MoveRightVelocity);

        //
        // Check if player fires from guns.
        //
        if (m_FireDown)
        {
            m_SpaceShip->Fire();
        }

        //
        // Update scene and tick physics.
        //
        m_Scene->OnUpdate(deltaTime);
        m_Scene->Tick(deltaTime);

        //
        // Check if we are in deferred restart state.
        //
        if (m_IsRestarting)
        {
            DoRestart();
            m_IsRestarting = false;
        }
    }

    void Game::Render(float deltaTime) noexcept
    {
        auto renderSystem = Rendering::RenderSystem::Current;

        //
        // Prepare rendering to viewport.
        //
        renderSystem->BeginDrawViewport(m_Viewport);

        //
        // Do actual render.
        //
        (void)deltaTime;
        auto commandList = renderSystem->GetImmediateCommandList();
        m_Scene->OnRender(commandList);

        //
        // Present & flip.
        //
        renderSystem->EndDrawViewport(m_Viewport, true, 1);
    }

    void Game::Restart() noexcept
    {
        m_IsRestarting = true;
    }

    void Game::NotifyMeteoriteShotDown() noexcept
    {
        ++m_MeteoritesShotDown;

        RecomputeInterval();
    }

    void Game::DoRestart() noexcept
    {
        if (m_Scene != nullptr)
        {
            m_Scene->Clear();
        }

        m_SpaceShip = nullptr;
        m_Scene = nullptr;

        //
        // Create scene with zero gravity.
        //
        m_Scene = World::Physics::MakeScene(DirectX::XMFLOAT3(0.0F, 0.0F, 0.0F));

        //
        // Setup scene camera.
        //
        auto camera = m_Scene->GetCamera();
        camera->SetLens(DirectX::XMConvertToRadians(45.0F), 16.0F / 9.0F, 0.01F, 1000.0F);
        camera->LookAt(
            DirectX::XMVectorSet(0.0F, 30.0F, 5.0F, 0.0F),
            DirectX::XMVectorSet(0.0F, 0.0F, 10.0F, 0.0F),
            DirectX::XMVectorSet(0.0F, 1.0F, 0.0F, 0.0F)
        );

        //
        // Make spaceship.
        //
        m_SpaceShip = MakeRef<SpaceShip>(
            m_Scene.Get(),
            m_SpaceShipMesh,
            m_SpaceShipMaterial,
            m_BulletMesh,
            m_BulletMaterial
            );

        //
        // Add spaceship to scene.
        //
        m_Scene->Add(m_SpaceShip);

        //
        // Setup spawning.
        //
        m_SpawnTimeout = 0.0F;
        m_SpawnInterval = 0.25F;

        //
        // Reset counters.
        //
        m_MeteoritesShotDown = 0;

        //
        // Well...
        //
        RecomputeInterval();
    }

    void Game::RecomputeInterval() noexcept
    {
        const auto ranged = static_cast<float>(Clamp<uint32_t>(
            static_cast<uint32_t>(m_MeteoritesShotDown / 1.7F),
            1,
            150
            ));

        //
        // Funny expotential equation :)
        //
        const auto base = 0.981389F;
        const auto expe = -0.0175394F * ranged;
        const auto interval = base * std::exp(expe);
        const auto result = Clamp<float>(0.07F + 0.7F * interval, 0.05F, 0.5F);

        CORE_TRACE_MESSAGE(Debug, "Ranged: %f, interval: %f, result: %f", ranged, interval, result);
        
        m_SpawnInterval = result;
    }

    void Game::Initialize() noexcept
    {
        auto application = CoreApplication::Current;
        auto renderSystem = Rendering::RenderSystem::Current;

        //
        // Instantiate window.
        // 
        CoreWindowDesc desc{};
        desc.Width = 1280;
        desc.Height = 720;

        m_Window = application->MakeWindow(desc);
        m_Window->Show();

        //
        // Make viewport for allocated window.
        //
        m_Viewport = renderSystem->MakeViewport(m_Window->GetHandle(), desc.Width, desc.Height, false);

        //
        // Create sampler.
        //
        Rendering::SamplerDesc samplerDesc;
        samplerDesc.Desc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
        samplerDesc.Desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.Desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.Desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

        auto defaultSampler = renderSystem->MakeSampler(samplerDesc);

        //
        // Meteorite resoureces.
        //
        m_MeteoriteMaterial = MakeRef<Rendering::MaterialRenderer>(
            "./shaders/DiffuseMaterial.ps.cso",
            "./shaders/DiffuseMaterial.vs.cso"
            );
        m_MeteoriteMaterial->SetDiffuseColor(DirectX::Colors::Silver);
        m_MeteoriteMaterial->SetTextureSampler(defaultSampler);
        m_MeteoriteMaterial->SetTexture(renderSystem->MakeTexture2D("assets/textures/meteorite.dds"));
        m_MeteoriteMesh = MakeRef<Rendering::MeshRenderer>();

        //
        // Spaceship resources.
        //
        m_SpaceShipMaterial = MakeRef<Rendering::MaterialRenderer>(
            "./shaders/DiffuseMaterial.ps.cso",
            "./shaders/DiffuseMaterial.vs.cso"
            );
        m_SpaceShipMaterial->SetDiffuseColor(DirectX::Colors::LightSalmon);
        m_SpaceShipMaterial->SetTexture(renderSystem->MakeTexture2D("assets/textures/ship.dds"));
        m_SpaceShipMaterial->SetTextureSampler(defaultSampler);
        m_SpaceShipMesh = MakeRef<Rendering::MeshRenderer>();

        //
        // Bullet resources.
        //
        m_BulletMaterial = MakeRef<Rendering::MaterialRenderer>(
            "./shaders/EmissiveMaterial.ps.cso",
            "./shaders/EmissiveMaterial.vs.cso"
            );
        m_BulletMaterial->SetDiffuseColor(DirectX::Colors::LightSalmon);
        m_BulletMaterial->SetTexture(renderSystem->MakeTexture2D("assets/textures/bullet.dds"));
        m_BulletMaterial->SetTextureSampler(defaultSampler);
        m_BulletMesh = MakeRef<Rendering::MeshRenderer>();

        //
        // Just restart game :)
        //
        DoRestart();
    }

    void Game::Shutdown() noexcept
    {
        m_Viewport = nullptr;

        m_Window->Destroy();
        m_Window = nullptr;
    }

    DirectX::XMVECTOR XM_CALLCONV Game::RandomVector2D() noexcept
    {
        DirectX::XMFLOAT2A result;

        std::uniform_real<float> distribution{};

        result.x = distribution(m_RandomEngine);
        result.y = distribution(m_RandomEngine);

        return DirectX::XMVector2Normalize(DirectX::XMLoadFloat2A(&result));
    }

    DirectX::XMVECTOR XM_CALLCONV Game::RandomVector3D() noexcept
    {
        DirectX::XMFLOAT3A result;

        std::uniform_real<float> distribution{};

        result.x = distribution(m_RandomEngine);
        result.y = distribution(m_RandomEngine);
        result.z = distribution(m_RandomEngine);

        return DirectX::XMVector3Normalize(DirectX::XMLoadFloat3A(&result));
    }

    DirectX::XMVECTOR XM_CALLCONV Game::RandomVector3D(DirectX::FXMVECTOR min, DirectX::FXMVECTOR max) noexcept
    {
        DirectX::XMFLOAT3A vmin;
        DirectX::XMFLOAT3A vmax;

        DirectX::XMStoreFloat3A(&vmin, min);
        DirectX::XMStoreFloat3A(&vmax, max);

        std::uniform_real<float> distributionX{ vmin.x, vmax.x };
        std::uniform_real<float> distributionY{ vmin.y, vmax.y };
        std::uniform_real<float> distributionZ{ vmin.z, vmax.z };

        DirectX::XMFLOAT3A result;
        result.x = distributionX(m_RandomEngine);
        result.y = distributionY(m_RandomEngine);
        result.z = distributionZ(m_RandomEngine);
        
        return DirectX::XMLoadFloat3A(&result);
    }

    DirectX::XMVECTOR XM_CALLCONV Game::RandomUnitVector() noexcept
    {
        std::uniform_real<float> uniform{};

        //
        // Compute random `signum` for target axis vector.
        //
        auto signum = (uniform(m_RandomEngine) > 0.5F) ? 1.0F : -1.0F;

        DirectX::XMFLOAT3A axis;
        axis.x = uniform(m_RandomEngine);
        axis.y = uniform(m_RandomEngine);

        //
        // Square known two components.
        //
        const auto x2 = axis.x * axis.x;
        const auto y2 = axis.y * axis.y;

        //
        // And final z component.
        //
        axis.z = signum * std::sqrt(1.0F - x2 - y2);

        const auto result = DirectX::XMLoadFloat3A(&axis);
        return result;
    }

    DirectX::XMVECTOR XM_CALLCONV Game::RandomQuaternion() noexcept
    {
        std::uniform_real<float> uniform{};
        std::uniform_real<float> randomAngle{0.0F, DirectX::XM_2PI};


        //
        // Compute random angle.
        //
        const auto angle = randomAngle(m_RandomEngine);

        //
        // And random axis.
        //
        const auto axis = RandomUnitVector();

        //
        // And create quaternion from it.
        //
        const auto result = DirectX::XMQuaternionRotationAxis(axis, angle);
        return result;
    }

    DirectX::XMVECTOR XM_CALLCONV Game::RandomAngularVelocity(float max) noexcept
    {
        std::uniform_real<float> uniform{};

        DirectX::XMFLOAT3A vector;
        vector.x = uniform(m_RandomEngine);
        vector.y = uniform(m_RandomEngine);
        vector.z = uniform(m_RandomEngine);

        const auto result = DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&vector), max);
        return result;
    }

    float Game::RandomScalar(float min, float max) noexcept
    {
        std::uniform_real<float> distribution{ min, max };
        return distribution(m_RandomEngine);
    }

    void Game::SpawnMeteorite() noexcept
    {
        //
        // Spawn range is wide spreaded
        //
        auto spawnRange = RandomScalar(-VisibleRangeExtent, VisibleRangeExtent);

        //
        // However, target range is condense.
        //
        auto targetRange = RandomScalar(-VisibleRangeExtent, VisibleRangeExtent) * 0.5F;
        
        //
        // Copmute spawn point.
        //
        auto spawnPoint = DirectX::XMVectorSet(spawnRange, 0.0F, 40.0F, 0.0F);

        //
        // Compute desired hit target point.
        //
        auto hitPoint = DirectX::XMVectorSet(spawnRange + targetRange, 0.0F, 0.0F, 0.0F);

        //
        // And finally velocity vector.
        //
        auto velocityNormal = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(hitPoint, spawnPoint));

        //
        // Select some random velocity.
        //
        auto velocityValue = RandomScalar(10.0F, 15.0F);

        //
        // Compute resulting velocity.
        //
        auto velocity = DirectX::XMVectorScale(velocityNormal, velocityValue);

        auto size = RandomVector3D(
            DirectX::XMVectorSet(1.0F, 1.0F, 1.0F, 0.0F),
            DirectX::XMVectorSet(2.5F, 2.5F, 2.5F, 0.0F)
        );

        //
        // Compute random orientation quaternion.
        //
        auto randomAngularVelocity = RandomAngularVelocity(2.0F);

        //
        // Make meteorite.
        //
        auto meteorite = Meteorite::Make(
            spawnPoint,
            DirectX::XMQuaternionIdentity(),
            velocity,
            size,
            randomAngularVelocity,
            m_MeteoriteMesh,
            m_MeteoriteMaterial);

        //
        // And add it to scene.
        //
        m_Scene->Add(meteorite);
    }
}
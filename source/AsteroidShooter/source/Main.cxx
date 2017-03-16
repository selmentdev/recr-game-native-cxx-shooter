//
// Copyright (C) Selmentdev, 2017
//
//      See LICENSE file in the project root for full license information.
//

#include <Windows.h>
#include <Core/Environment.hxx>
#include <Core.Diagnostics/Trace.hxx>
#include <Core.Diagnostics/Debug.hxx>
#include <Core/CoreApplication.hxx>
#include <Core/CoreWindow.hxx>
#include <Core.Rendering/RenderSystem.hxx>
#include <Core/Timer.hxx>
#include <Core/FileSystem.hxx>
#include <Core.World/Physics.hxx>
#include <algorithm>

//
// Sorry. It was easier this way :P
//
#ifdef NDEBUG
#pragma comment(lib, "PhysX3_x64.lib")
#pragma comment(lib, "PhysX3Common_x64.lib")
#pragma comment(lib, "PhysX3Cooking_x64.lib")
#pragma comment(lib, "PhysX3CharacterKinematic_x64.lib")
#pragma comment(lib, "PhysX3Extensions.lib")
#pragma comment(lib, "PhysXProfileSDK.lib")
#else
#pragma comment(lib, "PhysX3DEBUG_x64.lib")
#pragma comment(lib, "PhysX3CommonDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CookingDEBUG_x64.lib")
#pragma comment(lib, "PhysX3CharacterKinematicDEBUG_x64.lib")
#pragma comment(lib, "PhysX3ExtensionsDEBUG.lib")
#pragma comment(lib, "PhysXProfileSDKDEBUG.lib")
#endif

//
// Game definition.
//
#include <Game.hxx>

INT WINAPI WinMain(
    HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszCommandLine,
    INT nShowCommand
)
{
    {
        //
        // Well, unused.
        //
        (void)hPrevInstance;
        (void)lpszCommandLine;
        (void)nShowCommand;

        //
        // Initialize basic environment.
        //
        Core::Environment::Initialize(hThisInstance);

        //
        // Initialize physics.
        //
        Core::World::Physics::Initialize();
        {
            //
            // Run application framework.
            //
            auto application = Core::CoreApplication::MakeApplication();
            {
                //
                // Instantiate render system.
                //
                // Note:
                //
                //      This render system implementation is almost ready to be abstracted over
                //      more rendering APIs than Direct3D 11 used in this application.
                //
                auto renderSystem = Core::Rendering::RenderSystem::MakeRenderSystem();

                //
                // Instantiate game.
                //
                auto game = Core::MakeRef<GameProject::Game>();
                application->SetEventHandler(game);
                {

                    //
                    // Pump some messages: somehow my laptop didn't refreshed window content
                    // until I clicked Alt+Enter to enter fullscreen.
                    //
                    Core::CoreApplication::PumpMessages();

                    //
                    // All systems working. Instantiate game.
                    //
                    game->Initialize();

                    //
                    // Start up timer.
                    //
                    Core::Timer timer{};
                    timer.Start();

                    //
                    // Tick timer.
                    //
                    timer.Tick();

                    //
                    // Work until user requests exit.
                    //
                    while (!Core::Environment::IsExitRequested())
                    {
                        //
                        // Tick timer.
                        //
                        timer.Tick();

                        //
                        // Compute current delta time.
                        //
                        auto deltaTime = timer.DeltaTime();

                        //
                        // Clamp it to sane range.
                        //
                        deltaTime = (std::min)((std::max)(0.0F, deltaTime), 0.05F);

                        //
                        // Update application.
                        //
                        application->Tick(deltaTime);

                        //
                        // And render system.
                        //
                        renderSystem->Tick(deltaTime);

                        //
                        // And game.
                        //
                        game->Tick(deltaTime);

                        //
                        // Render game objects.
                        //
                        game->Render(deltaTime);

                        //
                        // Pump messages.
                        //
                        Core::CoreApplication::PumpMessages();
                    }

                    //
                    // Shutdown game.
                    //
                    game->Shutdown();
                }
            }
        }

        //
        // Shutdown physics.
        //
        Core::World::Physics::Shutdown();

        //
        // And basic environment.
        //
        Core::Environment::Shutdown();
    }
    return 0;
}
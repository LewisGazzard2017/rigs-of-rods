/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013+     Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Application.h"
#include "RoRPrerequisites.h"
#include "MainThread.h"
#include "Language.h"
#include "ErrorUtils.h"
#include "Utils.h"
#include "PlatformUtils.h"
#include "Settings.h"

#include "Application.h"
#include "Beam.h"
#include "BeamEngine.h"
#include "BeamFactory.h"
#include "CacheSystem.h"
#include "CameraManager.h"
#include "Character.h"
#include "CharacterFactory.h"
#include "ChatSystem.h"
#include "ContentManager.h"
#include "DashBoardManager.h"
#include "DustManager.h"
#include "ErrorUtils.h"
#include "ForceFeedback.h"
#include "GlobalEnvironment.h"
#include "GUIManager.h"
#include "GUI_LoadingWindow.h"
#include "GUI_MainSelector.h"
#include "GUI_MultiplayerClientList.h"
#include "GUI_MultiplayerSelector.h"
#include "Heathaze.h"
#include "InputEngine.h"
#include "Language.h"
#include "MumbleIntegration.h"
#include "Mirrors.h"
#include "Network.h"
#include "OgreSubsystem.h"
#include "OverlayWrapper.h"
#include "OutProtocol.h"
#include "PlayerColours.h"
#include "RoRFrameListener.h"
#include "Scripting.h"
#include "Settings.h"
#include "Skin.h"
#include "SoundScriptManager.h"
#include "SurveyMapManager.h"
#include "TerrainManager.h"
#include "TruckHUD.h"
#include "Utils.h"
#include "SkyManager.h"

#include <OgreException.h>
#include <OgreRoot.h>

#ifdef USE_CURL
#   include <curl/curl.h>
#endif //USE_CURL

GlobalEnvironment *gEnv; // Global instance used throughout the game.

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char *argv[])
{
    using namespace RoR;

#ifdef USE_CURL
    curl_global_init(CURL_GLOBAL_ALL); // MUST init before any threads are started
#endif

    try
    {
        gEnv = new GlobalEnvironment(); // Instantiate global environment. TODO: Eliminate gEnv
        App::Init();

        // Detect system paths

        int res = RoR::System::DetectBasePaths(); // Updates globals
        if (res == -1)
        {
            ErrorUtils::ShowError(_L("Startup error"), _L("Error while retrieving program directory path"));
            return -1;
        }
        else if (res == -2)
        {
            ErrorUtils::ShowError(_L("Startup error"), _L("Error while retrieving user directory path"));
            return -1;
        }

        // Create OGRE default logger early.

        App::SetSysLogsDir(App::GetSysUserDir() + PATH_SLASH + "logs");

        auto ogre_log_manager = OGRE_NEW Ogre::LogManager();
        Ogre::String log_filepath = App::GetSysLogsDir() + PATH_SLASH + "RoR.log";
        ogre_log_manager->createLog(log_filepath, true, true);
        App::SetDiagTraceGlobals(true); // We have logger -> we can trace.

        // Setup program paths

        if (! Settings::SetupAllPaths()) // Updates globals
        {
            ErrorUtils::ShowError(_L("Startup error"), _L("Resources folder not found. Check if correctly installed."));
            return -1;
        }

        App::GetSettings().LoadSettings(App::GetSysConfigDir() + PATH_SLASH + "RoR.cfg"); // Main config file

        // Process command-line arguments

#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE //MacOSX adds an extra argument in the form of -psn_0_XXXXXX when the app is double clicked
        RoR::App::GetSettings().ProcessCommandLine(argc, argv);
#endif

        if (App::GetPendingAppState() == App::APP_STATE_PRINT_HELP_EXIT)
        {
            ShowCommandLineUsage();
            return 0;
        }
        if (App::GetPendingAppState() == App::APP_STATE_PRINT_VERSION_EXIT)
        {
            ShowVersion();
            return 0;
        }

#ifdef USE_CRASHRPT
        InstallCrashRpt();
#endif //USE_CRASHRPT

        // ================================================================================
        // Bootstrap
        // ================================================================================

        App::StartOgreSubsystem();
    #ifdef ROR_USE_OGRE_1_9
        Ogre::OverlaySystem* overlay_system = new Ogre::OverlaySystem(); //Overlay init
    #endif

        App::CreateContentManager();

        LanguageEngine::getSingleton().setup();

        // Add startup resources
        RoR::App::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::OGRE_CORE);
        RoR::App::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::GUI_STARTUP_SCREEN);
        RoR::App::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::GUI_MENU_WALLPAPERS);
        Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
        Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Wallpapers");

        // Setup rendering (menu + simulation)
        Ogre::SceneManager* scene_manager = RoR::App::GetOgreSubsystem()->GetOgreRoot()->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "main_scene_manager");
        gEnv->sceneManager = scene_manager;
    #ifdef ROR_USE_OGRE_1_9
        if (overlay_system)
        {
            scene_manager->addRenderQueueListener(overlay_system);
        }
    #endif

        Ogre::Camera* camera = scene_manager->createCamera("PlayerCam");
        camera->setPosition(Ogre::Vector3(128, 25, 128)); // Position it at 500 in Z direction
        camera->lookAt(Ogre::Vector3(0, 0, -300)); // Look back along -Z
        camera->setNearClipDistance(0.5);
        camera->setFarClipDistance(1000.0 * 1.733);
        camera->setFOVy(Ogre::Degree(60));
        camera->setAutoAspectRatio(true);
        RoR::App::GetOgreSubsystem()->GetViewport()->setCamera(camera);
        gEnv->mainCamera = camera;

        // SHOW BOOTSTRAP SCREEN

        // Create rendering overlay
        Ogre::OverlayManager& overlay_manager = Ogre::OverlayManager::getSingleton();
        Ogre::Overlay* startup_screen_overlay = static_cast<Ogre::Overlay*>(overlay_manager.getByName("RoR/StartupScreen"));
        if (!startup_screen_overlay)
        {
            OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Cannot find loading overlay for startup screen", "MainThread::Go");
        }

        // Set random wallpaper image
        //is this still needed? As things load so fast that it's rendred for a fraction of a second.
        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("RoR/StartupScreenWallpaper");
        Ogre::String menu_wallpaper_texture_name = GUIManager::getRandomWallpaperImage(); // TODO: manage by class Application
        if (! menu_wallpaper_texture_name.empty() && ! mat.isNull())
        {
            if (mat->getNumTechniques() > 0 && mat->getTechnique(0)->getNumPasses() > 0 && mat->getTechnique(0)->getPass(0)->getNumTextureUnitStates() > 0)
            {
                mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(menu_wallpaper_texture_name);
            }
        }

        startup_screen_overlay->show();

        scene_manager->clearSpecialCaseRenderQueues();
        scene_manager->addSpecialCaseRenderQueue(Ogre::RENDER_QUEUE_OVERLAY);
        scene_manager->setSpecialCaseRenderQueueMode(Ogre::SceneManager::SCRQM_INCLUDE);

        App::GetOgreSubsystem()->GetOgreRoot()->renderOneFrame(); // Render bootstrap screen once and leave it visible.

        RoR::App::CreateCacheSystem();

        RoR::App::GetCacheSystem()->setLocation(App::GetSysCacheDir() + PATH_SLASH, App::GetSysConfigDir() + PATH_SLASH);

        App::GetContentManager()->init();

        // HIDE BOOTSTRAP SCREEN

        startup_screen_overlay->hide();

        // Back to full rendering
        scene_manager->clearSpecialCaseRenderQueues();
        scene_manager->setSpecialCaseRenderQueueMode(Ogre::SceneManager::SCRQM_EXCLUDE);

        App::CreateGuiManagerIfNotExists();

        // Load and show menu wallpaper
        MyGUI::VectorWidgetPtr v = MyGUI::LayoutManager::getInstance().loadLayout("wallpaper.layout");
        MyGUI::Widget* menu_wallpaper_widget = nullptr;
        if (!v.empty())
        {
            MyGUI::Widget* mainw = v.at(0);
            if (mainw)
            {
                MyGUI::ImageBox* img = (MyGUI::ImageBox *)(mainw->getChildAt(0));
                if (img)
                    img->setImageTexture(menu_wallpaper_texture_name);
                menu_wallpaper_widget = mainw;
            }
        }

    #ifdef USE_ANGELSCRIPT

        new ScriptEngine(); // Init singleton. TODO: Move under Application

    #endif

        RoR::App::CreateInputEngine();
        RoR::App::GetInputEngine()->setupDefault(RoR::App::GetOgreSubsystem()->GetMainHWND());

        // Initialize "managed materials"
        // These are base materials referenced by user content
        // They must be initialized before any content is loaded, including mod-cache update.
        // Otherwise material links are unresolved and loading ends with an exception
        // TODO: Study Ogre::ResourceLoadingListener and implement smarter solution (not parsing materials on cache refresh!)
        App::GetContentManager()->InitManagedMaterials();

        if (BSETTING("regen-cache-only", false)) //Can be usefull so we will leave it here -max98
        {
            App::GetContentManager()->RegenCache();
            App::SetPendingAppState(App::APP_STATE_SHUTDOWN);
        }
            

        RoR::App::GetCacheSystem()->Startup();

        // Create legacy RoRFrameListener
        auto* frame_listener = new RoRFrameListener();

    #ifdef USE_ANGELSCRIPT
        ScriptEngine::getSingleton().SetFrameListener(frame_listener);
    #endif

    #ifdef USE_MPLATFORM
	    m_frame_listener->m_platform = new MPlatform_FD();
	    if (m_frame_listener->m_platform) 
	    {
		    m_platform->connect();
	    }
    #endif

        frame_listener->windowResized(RoR::App::GetOgreSubsystem()->GetRenderWindow());
        RoRWindowEventUtilities::addWindowEventListener(RoR::App::GetOgreSubsystem()->GetRenderWindow(), frame_listener);

    #ifdef _WIN32
        if (App::GetIoFFbackEnabled()) // Force feedback
        {
            if (RoR::App::GetInputEngine()->getForceFeedbackDevice())
            {
                frame_listener->m_forcefeedback.Setup();
            }
            else
            {
                LOG("No force feedback device detected, disabling force feedback");
                App::SetIoFFbackEnabled(false);
            }
        }
    #endif // _WIN32

        // initiate player colours
        PlayerColours::getSingleton();

        // new factory for characters, net is INVALID, will be set later
        new CharacterFactory();

        new BeamFactory();

        MainThread main_obj(frame_listener);
        

        // ========================================================================
        // Main loop (switches application states)
        // ========================================================================

        App::State previous_application_state = App::GetActiveAppState();
        App::SetActiveAppState(App::APP_STATE_MAIN_MENU);

        if (! App::GetDiagPreselectedTerrain().empty())
        {
            App::SetPendingAppState(App::APP_STATE_SIMULATION);
        }
        if (App::GetPendingMpState() == App::MP_STATE_CONNECTED)
        {
            main_obj.JoinMultiplayerServer();
        }
        for (;;)
        {
            if (App::GetPendingAppState() == App::APP_STATE_SHUTDOWN)
            {
                break;
            }
            else if (App::GetPendingAppState() == App::APP_STATE_MAIN_MENU)
            {
                App::SetActiveAppState(App::APP_STATE_MAIN_MENU);
                App::SetPendingAppState(App::APP_STATE_NONE);

                if (previous_application_state == App::APP_STATE_SIMULATION)
                {
                    if (App::GetActiveMpState() == App::MP_STATE_CONNECTED)
                    {
                        RoR::Networking::Disconnect();
                        App::GetGuiManager()->SetVisible_MpClientList(false);
                    }
                    frame_listener->UnloadTerrain();
                    gEnv->cameraManager->OnReturnToMainMenu();
                    /* Hide top menu */
                    App::GetGuiManager()->SetVisible_TopMenubar(false);
                    /* Restore wallpaper */
                    menu_wallpaper_widget->setVisible(true);

                    /* Set Mumble to non-positional audio */
    #ifdef USE_MUMBLE
                    MumbleIntegration::getSingleton().update(
                        Ogre::Vector3::ZERO,
                        Ogre::Vector3(0.0f, 0.0f, 1.0f),
                        Ogre::Vector3(0.0f, 1.0f, 0.0f),
                        Ogre::Vector3::ZERO,
                        Ogre::Vector3(0.0f, 0.0f, 1.0f),
                        Ogre::Vector3(0.0f, 1.0f, 0.0f));
    #endif // USE_MUMBLE
                }

                if (App::GetAudioMenuMusic())
                {
                    SoundScriptManager::getSingleton().createInstance("tracks/main_menu_tune", -1, nullptr);
                    SoundScriptManager::getSingleton().trigStart(-1, SS_TRIG_MAIN_MENU);
                }

                App::GetGuiManager()->ReflectGameState();
                if (App::GetPendingMpState() == App::MP_STATE_CONNECTED || BSETTING("SkipMainMenu", false))
                {
                    // Multiplayer started from configurator / MainMenu disabled -> go directly to map selector (traditional behavior)
                    if (App::GetDiagPreselectedTerrain() == "")
                    {
                        RoR::App::GetGuiManager()->SetVisible_GameMainMenu(false);
                        RoR::App::GetGuiManager()->GetMainSelector()->Show(LT_Terrain);
                    }
                }

                main_obj.EnterMainMenuLoop();
            }
            else if (App::GetPendingAppState() == App::APP_STATE_SIMULATION)
            {
                if (frame_listener->SetupGameplayLoop())
                {
                    App::SetActiveAppState(App::APP_STATE_SIMULATION);
                    App::SetPendingAppState(App::APP_STATE_NONE);
                    App::GetGuiManager()->ReflectGameState();
                    frame_listener->EnterGameplayLoop();
                }
                else
                {
                    App::SetPendingAppState(App::APP_STATE_MAIN_MENU);
                }
            }
            else if (App::GetPendingAppState() == App::APP_STATE_CHANGE_MAP)
            {
                //Sim -> change map -> sim
                //                  -> back to menu

                App::SetActiveAppState(App::APP_STATE_CHANGE_MAP);
                App::SetPendingAppState(App::APP_STATE_NONE);
                if (previous_application_state == App::APP_STATE_SIMULATION)
                {
                    frame_listener->UnloadTerrain();
                }
                menu_wallpaper_widget->setVisible(true);

                if (App::GetDiagPreselectedTerrain() == "")
                {
                    RoR::App::GetGuiManager()->GetMainSelector()->Show(LT_Terrain);
                }
                else
                {
                    RoR::App::GetGuiManager()->SetVisible_GameMainMenu(true);
                }
                //It's the same thing so..
                main_obj.EnterMainMenuLoop();
            }
            previous_application_state = App::GetActiveAppState();
        } // End of app state loop

        // ========================================================================
        // Cleanup
        // ========================================================================

        App::GetSettings().SaveSettings(); // Save RoR.cfg

        RoR::App::GetGuiManager()->GetMainSelector()->~MainSelector();

    #ifdef USE_SOCKETW
        if (App::GetActiveMpState() == App::MP_STATE_CONNECTED)
        {
            RoR::Networking::Disconnect();
        }
    #endif //SOCKETW

        //TODO: we should destroy OIS here
        //TODO: we could also try to destroy SoundScriptManager, but we don't care!

    #ifdef USE_MPLATFORM
	    if (frame_listener->mplatform != nullptr)
	    {
		    if (frame_listener->mplatform->disconnect())
		    {
			    delete(frame_listener->mplatform);
			    frame_listener->mplatform = nullptr;
		    }
	    }
    #endif

        scene_manager->destroyCamera(camera);
        RoR::App::GetOgreSubsystem()->GetOgreRoot()->destroySceneManager(scene_manager);
    #ifdef ROR_USE_OGRE_1_9
        // Produces a segfault
        // delete overlay_system;
    #endif

        App::DestroyOverlayWrapper();

        App::DestroyContentManager();

        delete frame_listener;

        delete gEnv->cameraManager;
        gEnv->cameraManager = nullptr;

        delete gEnv;
        gEnv = nullptr;
    }
    catch (Ogre::Exception& e)
    {
        ErrorUtils::ShowError(_L("An exception has occured!"), e.getFullDescription());
    }
    catch (std::runtime_error& e)
    {
        ErrorUtils::ShowError(_L("An exception (std::runtime_error) has occured!"), e.what());
    }

#ifdef USE_CRASHRPT
    UninstallCrashRpt();
#endif //USE_CRASHRPT

    return 0;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
{
    return main(__argc, __argv);
}
#endif

#ifdef __cplusplus
}
#endif

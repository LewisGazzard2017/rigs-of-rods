/*
    This source file is part of Rigs of Rods
    Copyright 2013-2016 Petr Ohlidal & contributors

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

/** 
    @file   Application.cpp
    @author Petr Ohlidal
    @date   05/2014
*/

#include "Application.h"

#include <OgreException.h>

#include "CacheSystem.h"

#include "ContentManager.h"
#include "GUIManager.h"
#include "InputEngine.h"
#include "OgreSubsystem.h"
#include "OverlayWrapper.h"
#include "SceneMouse.h"

namespace RoR {

#define BOOL2STR(_B_) (_B_ ? "True" : "False")

template <typename value_T> GVar<value_T>::GVar(const char* _name, const char* _conf_name, value_T _def_val):
    m_name(_name),
    m_conf_name(_conf_name),
    m_config_val(_def_val),
    m_active_val(_def_val)
{}

void LogVarStr(const char* name, const char* type, const char* old_val, const char* new_val)
{
    if (App::diag_trace_globals.GetActive())
    {
        char buf[1000];
        sprintf(buf, "[RoR|GVars] %s (%s): [%s] => [%s]", name, type, old_val, new_val);
        LOG(buf);
    }
}

void LogVarPending(const char* name, const char* new_val)
{
    if (App::diag_trace_globals.GetActive())
    {
        char buf[1000];
        sprintf(buf, "[RoR|GVars] %s (pending): ==> [%s]", name, new_val);
        LOG(buf);
    }
}

void LogVarInt(const char* name, const char* type, int old_val, int new_val)
{
    if (App::diag_trace_globals.GetActive())
    {
        char buf[1000];
        sprintf(buf, "[RoR|GVars] %s (%s): [%d] => [%d]", name, type, old_val, new_val);
        LOG(buf);
    }
}

void LogVarIntPending(const char* name, int v)
{
    if (App::diag_trace_globals.GetActive())
    {
        char buf[1000];
        sprintf(buf, "[RoR|GVars] %s (pending): ==> [%d]", name, v);
        LOG(buf);
    }
}

void LogVarFloat(const char* name, const char* type, float old_val, float new_val)
{
    if (App::diag_trace_globals.GetActive())
    {
        char buf[1000];
        sprintf(buf, "[RoR|GVars] %s (%s): [%f] => [%f]", name, type, old_val, new_val);
        LOG(buf);
    }
}

void LogVarFloatPending(const char* name, float v)
{
    if (App::diag_trace_globals.GetActive())
    {
        char buf[1000];
        sprintf(buf, "[RoR|GVars] %s (pending): ==> [%f]", name, v);
        LOG(buf);
    }
}

void GVarStr::SetActive(std::string s)
{
    if (s != m_active_val)
    {
        LogVarStr(m_name, "active", m_active_val.c_str(), s.c_str());
        m_active_val = s; 
    }
}

void GVarStr::SetConfig(std::string s)
{
    if (s != m_config_val)
    {
        LogVarStr(m_name, "config", m_config_val.c_str(), s.c_str());
        m_config_val = s;
    }
}

void GVarStr::SetPending(std::string s)
{
    if (s != m_pending_val)
    {
        LogVarPending(m_name, s.c_str()); m_pending_val = s;
        m_has_pending = true;
    }
}

void GVarBool::SetActive(bool v)
{
    if (v != m_active_val)
    {
        LogVarStr(m_name, "active", BOOL2STR(m_active_val), BOOL2STR(v));
        m_active_val = v;
    }
}

void GVarBool::SetConfig(bool v)
{
    if (v != m_config_val)
    {
        LogVarStr(m_name, "config", BOOL2STR(m_active_val), BOOL2STR(v));
        m_config_val = v;
    }
}

void GVarBool::SetPending(bool v)
{
    if (v != m_pending_val)
    {
        LogVarPending(m_name, BOOL2STR(v));
        m_pending_val = v;
        m_has_pending = true;
    }
}

void GVarInt::SetActive(int v)
{
    if (v != m_active_val)
    {
        LogVarInt(m_name, "active", m_active_val, v);
        m_active_val = v;
    }
}

void GVarInt::SetConfig(int v)
{
    if (v != m_config_val)
    {
        LogVarInt(m_name, "active", m_config_val, v);
        m_config_val = v;
    }
}

void GVarInt::SetPending(int v)
{
    if (v != m_pending_val)
    {
        LogVarIntPending(m_name, v);
        m_pending_val = v;
        m_has_pending = true;
    }
}

void GVarFloat::SetActive(float v)
{
    if (v != m_active_val)
    {
        LogVarFloat(m_name, "active", m_active_val, v);
        m_active_val = v;
    }
}

void GVarFloat::SetConfig(float v)
{
    if (v != m_config_val)
    {
        LogVarFloat(m_name, "active", m_config_val, v);
        m_config_val = v;
    }
}

void GVarFloat::SetPending(float v)
{
    if (v != m_pending_val)
    {
        LogVarFloatPending(m_name, v);
        m_pending_val = v;
        m_has_pending = true;
    }
}

template <typename enum_T> GVarEnum<enum_T>::GVarEnum(const char* _name, const char* _conf_name, enum_T _def_val, EnumToStringFn _fn):
    GVar(_name, _conf_name, _def_val),
    m_tostring_fn(_fn)
{}

template <typename value_T> GVar::GVar(const char* _name, const char* _conf_name, value_T _def_val):
    m_name(_name),
    m_conf_name(_conf_name),
    m_config_val(_def_val),
    m_active_val(_def_val)
{}

template<typename enum_T> void GVarEnum::SetActive(enum_T v)
{
    if (v != m_active_val)
    {
        LogVarStr(m_name, "active", (*m_tostring_fn)((int)m_active_val), (*m_tostring_fn)((int)v));
        m_active_val = v;
    }
}

template<typename enum_T> void GVarEnum::SetConfig(enum_T v)
{
    if (v != m_config_val)
    {
        LogVarStr(m_name, "config", (*m_tostring_fn)((int)m_config_val), (*m_tostring_fn)((int)v));
        m_active_val = v;
    }
}

template<typename enum_T> void GVarEnum::SetPending(enum_T v)
{
    if (v != m_config_val)
    {
        LogVarPending(m_name, (*m_tostring_fn)((int)v));
        m_pending_val = v;
        m_has_pending = true;
    }
}

namespace App {


// ================================================================================
// Global variables
// ================================================================================

// Helpers
const char* SimGearboxModeToString(int v);
const char* GfxFlaresModeToString (int v);
const char* IoInputGrabModeToStr  (int v);
const char* GfxVegetationModeToStr(int v);
const char* GfxWaterModeToString  (int v);
const char* GfxSkyModeToString    (int v);
const char* GfxShadowModeToStr    (int v);
const char* GfxTexFilterToStr     (int v);
const char* AppStateToStr         (int v);
const char* SimStateToStr         (int v);
const char* MpStateToStr          (int v);

#define Q(x) #x /* http://stackoverflow.com/a/6671729 */
#define QUOTE(x) Q(x)
#define GVAR_DATA(_TYPE_, _NAME_, _CONF_, _DEFAULT_)       GVar##_TYPE_     _NAME_(QUOTE(_NAME_), _CONF_, _DEFAULT_)
#define GVAR_ENUM(_TYPE_, _NAME_, _CONF_, _DEFAULT_, _FN_) GVarEnum<_TYPE_> _NAME_(QUOTE(_NAME_), _CONF_, _DEFAULT_, _FN_)

// App
GVAR_ENUM( State           , app_state               , nullptr                   , APP_STATE_BOOTSTRAP, AppStateToStr);
GVAR_DATA( Bool            , app_multithread         , "Multi-threading"         , true     );
GVAR_DATA( Str             , app_language            , "Language"                , "English");
GVAR_DATA( Str             , app_locale              , "Language Short"          , "en"     );
GVAR_DATA( Str             , app_screenshot_format   , "Screenshot Format"       , "jpg"    );

// Simulation
GVAR_ENUM( SimState        , sim_state               , nullptr                   , SIM_STATE_NONE,   SimStateToStr         );
GVAR_ENUM( SimGearboxMode  , sim_gearbox_mode        , "GearboxMode"             , SIM_GEARBOX_AUTO, SimGearboxModeToString);
GVAR_DATA( Bool            , sim_position_storage    , "Position Storage"        , false   );
GVAR_DATA( Bool            , sim_replay_enabled      , "Replay mode"             , false   );
GVAR_DATA( Int             , sim_replay_length       , "Replay length"           , 1000    );
GVAR_DATA( Int             , sim_replay_stepping     , "Replay Steps per second" , 240     );
GVAR_DATA( Str             , sim_vehicle             , "Preselected Truck"       , nullptr );
GVAR_DATA( Str             , sim_vehicle_config      , "Preselected TruckConfig" , nullptr );
GVAR_DATA( Str             , sim_terrain             , "Preselected Map"         , nullptr );
GVAR_DATA( Bool            , sim_vehicle_enter       , "Enter Preselected Truck" , false   );

// Multiplayer
GVAR_ENUM( MpState         , mp_state                , nullptr                   , MP_STATE_DISABLED, MpStateToStr);
GVAR_DATA( Str             , mp_server_host          , "Server name"             , nullptr      );
GVAR_DATA( Int             , mp_server_port          , "Server port"             , 0            );
GVAR_DATA( Str             , mp_server_password      , "Server password"         , nullptr      );
GVAR_DATA( Str             , mp_player_name          , "Nickname"                , "Anonymous"  );

// Diagnostic
GVAR_DATA( Bool            , diag_trace_globals      , nullptr                                    , false );
GVAR_DATA( Bool            , diag_rig_log_node_import, "RigImporter_Debug_TraverseAndLogAllNodes" , false );
GVAR_DATA( Bool            , diag_rig_log_node_stats , "RigImporter_PrintNodeStatsToLog"          , false );
GVAR_DATA( Bool            , diag_rig_log_messages   , "RigImporter_PrintMessagesToLog"           , false );
GVAR_DATA( Bool            , diag_collisions         , "Debug Collisions"                         , false );
GVAR_DATA( Bool            , diag_truck_mass         , "Debug Truck Mass"        , false   );
GVAR_DATA( Bool            , diag_envmap             , "EnvMapDebug"             , false   );
GVAR_DATA( Bool            , diag_videocameras       , "VideoCameraDebug"        , false   );

// System
GVAR_DATA( Str             , sys_process_dir         , nullptr                   , nullptr );
GVAR_DATA( Str             , sys_user_dir            , nullptr                   , nullptr );
GVAR_DATA( Str             , sys_config_dir          , "Config Root"             , nullptr );
GVAR_DATA( Str             , sys_cache_dir           , "Cache Path"              , nullptr );
GVAR_DATA( Str             , sys_logs_dir            , "Log Path"                , nullptr );
GVAR_DATA( Str             , sys_resources_dir       , "Resources Path"          , nullptr );
GVAR_DATA( Str             , sys_profiler_dir        , "Profiler output dir"     , nullptr );
GVAR_DATA( Str             , sys_screenshot_dir      , nullptr                   , nullptr );

// Input - Output
GVAR_ENUM( IoInputGrabMode , io_input_grab_mode      , "Input Grab"              , INPUT_GRAB_ALL, IoInputGrabModeToStr);
GVAR_DATA( Bool            , io_ffback_enabled       , "Force Feedback"          , false           );
GVAR_DATA( Float           , io_ffback_camera_gain   , "Force Feedback Camera"   , 100.f           );
GVAR_DATA( Float           , io_ffback_center_gain   , "Force Feedback Centering", 0.f             );
GVAR_DATA( Float           , io_ffback_master_gain   , "Force Feedback Gain"     , 100.f           );
GVAR_DATA( Float           , io_ffback_stress_gain   , "Force Feedback Stress"   , 100.f           );
GVAR_DATA( Bool            , io_arcade_controls      , "ArcadeControls"          , true            );
GVAR_DATA( Int             , io_outgauge_mode        , "OutGauge Mode"           , 0               );
GVAR_DATA( Str             , io_outgauge_ip          , "OutGauge IP"             , "192.168.1.100" );
GVAR_DATA( Int             , io_outgauge_port        , "OutGauge Port"           , 1337            );
GVAR_DATA( Float           , io_outgauge_delay       , "OutGauge Delay"          , 10.f            );
GVAR_DATA( Int             , io_outgauge_id          , "OutGauge ID"             , 0               );

// Audio
GVAR_DATA( Float           , audio_master_volume     , "Sound Volume"            , 100.f   );
GVAR_DATA( Bool            , audio_enable_creak      , "Creak Sound"             , false   );
GVAR_DATA( Str             , audio_device_name       , "AudioDevice"             , nullptr );
GVAR_DATA( Bool            , audio_menu_music        , "MainMenuMusic"           , true    );

// Graphics
GVAR_ENUM( GfxFlaresMode   , gfx_flares_mode         , "Lights"                  , GFX_FLARES_ALL_VEHICLES_HEAD_ONLY , GfxFlaresModeToString );
GVAR_ENUM( GfxShadowType   , gfx_shadow_type         , "Shadow technique"        , GFX_SHADOW_TYPE_PSSM              , GfxShadowModeToStr );
GVAR_ENUM( GfxExtCamMode   , gfx_extcam_mode         , "External Camera Mode"    , GFX_EXTCAM_MODE_PITCHING          ,  );
GVAR_ENUM( GfxSkyMode      , gfx_sky_mode            , "Sky effects"             , GFX_SKY_SANDSTORM                 , GfxSkyModeToString );
GVAR_ENUM( GfxTexFilter    , gfx_texture_filter      , "Texture Filtering"       , GFX_TEXFILTER_TRILINEAR           , GfxTexFilterToStr );
GVAR_ENUM( GfxVegetation   , gfx_vegetation_mode     , "Vegetation"              , GFX_VEGETATION_NONE               , GfxVegetationModeToStr );
GVAR_ENUM( GfxWaterMode    , gfx_water_mode          , "Water effects"           , GFX_WATER_BASIC                   , GfxWaterModeToString );


GVAR_DATA( Bool            , gfx_enable_sunburn      , "Sunburn"                 , false  );
GVAR_DATA( Bool            , gfx_water_waves         , "Waves"                   , false  );
GVAR_DATA( Bool            , gfx_minimap_disabled    , "disableOverViewMap"      , false  );
GVAR_DATA( Int             , gfx_particles_mode      , "Particles"               , 1      );
GVAR_DATA( Bool            , gfx_enable_glow         , "Glow"                    , false  );
GVAR_DATA( Bool            , gfx_enable_hdr          , "HDR"                     , false  );
GVAR_DATA( Bool            , gfx_enable_heathaze     , "HeatHaze"                , false  );
GVAR_DATA( Bool            , gfx_envmap_enabled      , "Envmap"                  , true   );
GVAR_DATA( Int             , gfx_envmap_rate         , "EnvmapUpdateRate"        , 2      );
GVAR_DATA( Int             , gfx_skidmarks_mode      , "Skidmarks"               , 1      );
GVAR_DATA( Float           , gfx_sight_range         , "SightRange"              , 3000.f ); // Previously either 2000 or 4500 (inconsistent)
GVAR_DATA( Float           , gfx_fov_external        , "FOV External"            , 60.f   );
GVAR_DATA( Float           , gfx_fov_internal        , "FOV Internal"            , 75.f   );
GVAR_DATA( Int             , gfx_fps_limit           , "FPS-Limiter"             , 0      ); // Unlimited


// ================================================================================
// Global objects
// ================================================================================


// Object instances
static OgreSubsystem*   g_ogre_subsystem;
static ContentManager*  g_content_manager;
static OverlayWrapper*  g_overlay_wrapper;
static SceneMouse*      g_scene_mouse;
static GUIManager*      g_gui_manager;
static Console*         g_console;
static InputEngine*     g_input_engine;
static CacheSystem*     g_cache_system;
static MainThread*      g_main_thread_logic;

// Instance access
OgreSubsystem*         GetOgreSubsystem      () { return g_ogre_subsystem; };
Settings&              GetSettings           () { return Settings::getSingleton(); } // Temporary solution
ContentManager*        GetContentManager     () { return g_content_manager;}
OverlayWrapper*        GetOverlayWrapper     () { return g_overlay_wrapper;}
SceneMouse*            GetSceneMouse         () { return g_scene_mouse;}
GUIManager*            GetGuiManager         () { return g_gui_manager;}
Console*               GetConsole            () { return g_gui_manager->GetConsole();}
InputEngine*           GetInputEngine        () { return g_input_engine;}
CacheSystem*           GetCacheSystem        () { return g_cache_system;}
MainThread*            GetMainThreadLogic    () { return g_main_thread_logic;}

// Instance management
void SetMainThreadLogic(MainThread* obj) { g_main_thread_logic = obj; }

void StartOgreSubsystem()
{
	g_ogre_subsystem = new OgreSubsystem();
	if (g_ogre_subsystem == nullptr)
	{
		throw std::runtime_error("[RoR] Failed to create OgreSubsystem");
	}

	if (! g_ogre_subsystem->StartOgre("", ""))
	{
		throw std::runtime_error("[RoR] Failed to start up OGRE 3D engine");
	}
}

void ShutdownOgreSubsystem()
{
	assert(g_ogre_subsystem != nullptr && "ShutdownOgreSubsystem(): Ogre subsystem was not started");
	delete g_ogre_subsystem;
	g_ogre_subsystem = nullptr;
}

void CreateContentManager()
{
	g_content_manager = new ContentManager();
}

void DestroyContentManager()
{
	assert(g_content_manager != nullptr && "DestroyContentManager(): ContentManager never created");
	delete g_content_manager;
	g_content_manager = nullptr;
}

void CreateOverlayWrapper()
{
	g_overlay_wrapper = new OverlayWrapper();
	if (g_overlay_wrapper == nullptr)
	{
		throw std::runtime_error("[RoR] Failed to create OverlayWrapper");
	}
}

void DestroyOverlayWrapper()
{
	assert(g_overlay_wrapper != nullptr && "DestroyOverlayWrapper(): OverlayWrapper never created");
	delete g_overlay_wrapper;
	g_overlay_wrapper = nullptr;
}

void CreateSceneMouse()
{
	assert (g_scene_mouse == nullptr);
	g_scene_mouse = new SceneMouse();
}

void DeleteSceneMouse()
{
	assert (g_scene_mouse != nullptr);
	delete g_scene_mouse;
	g_scene_mouse = nullptr;
}

void CreateGuiManagerIfNotExists()
{
	if (g_gui_manager == nullptr)
	{
		g_gui_manager = new GUIManager();
	}
}

void DeleteGuiManagerIfExists()
{
	if (g_gui_manager != nullptr)
	{
		delete g_gui_manager;
		g_gui_manager = nullptr;
	}
}

void CreateInputEngine()
{
	assert(g_input_engine == nullptr);
	g_input_engine = new InputEngine();
}

void CreateCacheSystem()
{
	assert(g_cache_system == nullptr);
	g_cache_system = new CacheSystem();
}

// ================================================================================
// Private helper functions
// ================================================================================

const char* AppStateToStr(int v)
{
    switch ((State)v)
    {
    case App::APP_STATE_NONE:                return "NONE";
    case App::APP_STATE_BOOTSTRAP:           return "BOOTSTRAP";
    case App::APP_STATE_CHANGE_MAP:          return "CHANGE_MAP";
    case App::APP_STATE_MAIN_MENU:           return "MAIN_MENU";
    case App::APP_STATE_PRINT_HELP_EXIT:     return "PRINT_HELP_EXIT";
    case App::APP_STATE_PRINT_VERSION_EXIT:  return "PRINT_VERSION_EXIT";
    case App::APP_STATE_SHUTDOWN:            return "SHUTDOWN";
    case App::APP_STATE_SIMULATION:          return "SIMULATION";
    default:                                 return "~invalid~";
    }
}

const char* MpStateToStr(int v)
{
    switch ((MpState)v)
    {
    case App::MP_STATE_NONE:      return "NONE";
    case App::MP_STATE_DISABLED:  return "DISABLED";
    case App::MP_STATE_CONNECTED: return "CONNECTED";
    default:                      return "~invalid~";
    }
}

const char* SimStateToStr(int v)
{
    switch ((SimState)v)
    {
    case App::SIM_STATE_NONE       : return "NONE";
    case App::SIM_STATE_RUNNING    : return "RUNNING";
    case App::SIM_STATE_PAUSED     : return "PAUSED";
    case App::SIM_STATE_SELECTING  : return "SELECTING";
    case App::SIM_STATE_EDITOR_MODE: return "EDITOR_MODE";
    default                        : return "~invalid~";
    }
}

const char* SimGearboxModeToString(int v)
{
    switch ((SimGearboxMode)v)
    {
    case SIM_GEARBOX_AUTO         : return "AUTO";
    case SIM_GEARBOX_SEMI_AUTO    : return "SEMI_AUTO";
    case SIM_GEARBOX_MANUAL       : return "MANUAL";
    case SIM_GEARBOX_MANUAL_STICK : return "MANUAL_STICK";
    case SIM_GEARBOX_MANUAL_RANGES: return "MANUAL_RANGES";
    default                       : return "~invalid~";
    }
}

const char* GfxFlaresModeToString(int v)
{
    switch ((GfxFlaresMode)v)
    {
    case GFX_FLARES_NONE                   : return "NONE"                   ;
    case GFX_FLARES_NO_LIGHTSOURCES        : return "NO_LIGHTSOURCES"        ;
    case GFX_FLARES_CURR_VEHICLE_HEAD_ONLY : return "CURR_VEHICLE_HEAD_ONLY" ;
    case GFX_FLARES_ALL_VEHICLES_HEAD_ONLY : return "ALL_VEHICLES_HEAD_ONLY" ;
    case GFX_FLARES_ALL_VEHICLES_ALL_LIGHTS: return "ALL_VEHICLES_ALL_LIGHTS";
    default                                : return "~invalid~";
    }
}

const char* GfxVegetationModeToStr (int v)
{
    switch((GfxWaterMode)v)
    {
    case GFX_VEGETATION_NONE   : return "NONE";
    case GFX_VEGETATION_20PERC : return "20%";
    case GFX_VEGETATION_50PERC : return "50%";
    case GFX_VEGETATION_FULL   : return "FULL";
    default                    : return "~invalid~";
    }
}

const char* GfxWaterModeToString (int v)
{
    switch((GfxWaterMode)v)
    {
    case GFX_WATER_NONE      : return "NONE";
    case GFX_WATER_BASIC     : return "BASIC";
    case GFX_WATER_REFLECT   : return "REFLECT";
    case GFX_WATER_FULL_FAST : return "FULL_FAST";
    case GFX_WATER_FULL_HQ   : return "FULL_HQ";
    case GFX_WATER_HYDRAX    : return "HYDRAX";
    default                  : return "~invalid~";
    }
}

const char* GfxSkyModeToString (int v)
{
    switch((GfxSkyMode)v)
    {
    case GFX_SKY_SANDSTORM: return "SANDSTORM";
    case GFX_SKY_CAELUM   : return "CAELUM";
    case GFX_SKY_SKYX     : return "SKYX";
    default               : return "~invalid~";
    }
}

const char* IoInputGrabModeToStr(int v)
{
    switch ((IoInputGrabMode)v)
    {
    case INPUT_GRAB_NONE   : return "NONE"   ;
    case INPUT_GRAB_ALL    : return "ALL"    ;
    case INPUT_GRAB_DYNAMIC: return "DYNAMIC";
    default                : return "~invalid~";
    }
}

const char* GfxShadowModeToStr(int v)
{
    switch((GfxShadowType)v)
    {
    case GFX_SHADOW_TYPE_NONE   : return "NONE";
    case GFX_SHADOW_TYPE_TEXTURE: return "TEXTURE";
    case GFX_SHADOW_TYPE_PSSM   : return "PSSM";
    default                     : return "~invalid~";
    }
}

const char* GfxTexFilterToStr(int v)
{
    switch ((GfxTexFilter)v)
    {
    case GFX_TEXFILTER_NONE       : return "NONE";
    case GFX_TEXFILTER_BILINEAR   : return "BILINEAR";
    case GFX_TEXFILTER_TRILINEAR  : return "TRILINEAR";
    case GFX_TEXFILTER_ANISOTROPIC: return "ANISOTROPIC";
    default                       : return "~invalid~";
    }
}

} // namespace Application
} // namespace RoR

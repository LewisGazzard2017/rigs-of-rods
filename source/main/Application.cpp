/*
	This source file is part of Rigs of Rods
	Copyright 2013-2016 Petr Ohlidal

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
#define MK_GVAR_DATA(_TYPE_, _NAME_, _CONF_, _DEFAULT_)       GVar##_TYPE_     _NAME_(QUOTE(_NAME_), _CONF_, _DEFAULT_)
#define MK_GVAR_ENUM(_TYPE_, _NAME_, _CONF_, _DEFAULT_, _FN_) GVarEnum<_TYPE_> _NAME_(QUOTE(_NAME_), _CONF_, _DEFAULT_, _FN_)

// App
MK_GVAR_ENUM(State      , app_state             , nullptr,          APP_STATE_BOOTSTRAP, AppStateToStr);
MK_GVAR_DATA(Bool       , app_multithread       , "Multi-threading", true);
MK_GVAR_DATA(Str        , app_language          , "Language",       "English");
MK_GVAR_DATA(Str        , app_locale            , "Language Short", "en");
MK_GVAR_DATA(Str        , app_screenshot_format , "Screenshot Format", "png");

// Simulation
MK_GVAR_ENUM(SimState       , sim_state             , nullptr                   , SIM_STATE_NONE,   SimStateToStr         );
MK_GVAR_ENUM(SimGearboxMode , sim_gearbox_mode      , "GearboxMode"             , SIM_GEARBOX_AUTO, SimGearboxModeToString);
MK_GVAR_DATA(Bool           , sim_position_storage  , "Position Storage"        , false   );
MK_GVAR_DATA(Bool           , sim_replay_enabled    , "Replay mode"             , false   );
MK_GVAR_DATA(Int            , sim_replay_length     , "Replay length"           , 1000    );
MK_GVAR_DATA(Int            , sim_replay_stepping   , "Replay Steps per second" , 240     );
MK_GVAR_DATA(Str            , sim_vehicle           , "Preselected Truck"       , nullptr );
MK_GVAR_DATA(Str            , sim_vehicle_config    , "Preselected TruckConfig" , nullptr );
MK_GVAR_DATA(Str            , sim_terrain           , "Preselected Map"         , nullptr );
MK_GVAR_DATA(Bool           , sim_vehicle_enter     , "Enter Preselected Truck" , false   );

// Multiplayer
MK_GVAR_ENUM(MpState    , mp_state              , nullptr           , MP_STATE_DISABLED, MpStateToStr);
MK_GVAR_DATA(Str        , mp_server_host        , "Server name"     , nullptr      );
MK_GVAR_DATA(Int        , mp_server_port        , "Server port"     , 0            );
MK_GVAR_DATA(Str        , mp_server_password    , "Server password" , nullptr      );
MK_GVAR_DATA(Str        , mp_player_name        , "Nickname"        , "Anonymous"  );

// Diagnostic
MK_GVAR_DATA(Bool       , diag_trace_globals       , nullptr                                    , false );
MK_GVAR_DATA(Bool       , diag_rig_log_node_import , "RigImporter_Debug_TraverseAndLogAllNodes" , false );
MK_GVAR_DATA(Bool       , diag_rig_log_node_stats  , "RigImporter_PrintNodeStatsToLog"          , false );
MK_GVAR_DATA(Bool       , diag_rig_log_messages    , "RigImporter_PrintMessagesToLog"           , false );
MK_GVAR_DATA(Bool       , diag_collisions          , "Debug Collisions"                         , false );
MK_GVAR_DATA(Bool       , diag_truck_mass          , "Debug Truck Mass"                         , false );
MK_GVAR_DATA(Bool       , diag_envmap              , "EnvMapDebug"                              , false );
MK_GVAR_DATA(Bool       , diag_videocameras        , "VideoCameraDebug"                         , false );

// System
MK_GVAR_DATA(Str        , sys_process_dir          , nullptr               , nullptr );
MK_GVAR_DATA(Str        , sys_user_dir             , nullptr               , nullptr );
MK_GVAR_DATA(Str        , sys_config_dir           , "Config Root"         , nullptr );
MK_GVAR_DATA(Str        , sys_cache_dir            , "Cache Path"          , nullptr );
MK_GVAR_DATA(Str        , sys_logs_dir             , "Log Path"            , nullptr );
MK_GVAR_DATA(Str        , sys_resources_dir        , "Resources Path"      , nullptr );
MK_GVAR_DATA(Str        , sys_profiler_dir         , "Profiler output dir" , nullptr );
MK_GVAR_DATA(Str        , sys_screenshot_dir       , nullptr               , nullptr );

// Input - Output
MK_GVAR_DATA(Bool       , io_ffback_enabled        ///< Config: BOOL  Force Feedback
MK_GVAR_DATA(Float      , io_ffback_camera_gain    ///< Config: FLOAT Force Feedback Camera
MK_GVAR_DATA(Float      , io_ffback_center_gain    ///< Config: FLOAT Force Feedback Centering
MK_GVAR_DATA(Float      , io_ffback_master_gain    ///< Config: FLOAT Force Feedback Gain
MK_GVAR_DATA(Float      , io_ffback_stress_gain    ///< Config: FLOAT Force Feedback Stress
MK_GVAR_ENUM(IoInputGrabMode , io_input_grab_mode       ///< Config: BOOL  Input Grab          IoInputGrabModeToStr
MK_GVAR_DATA(Bool       , io_arcade_controls       ///< Config: BOOL  ArcadeControls
MK_GVAR_DATA(Int        , io_outgauge_mode         ///< Config: INT   OutGauge Mode
MK_GVAR_DATA(Str        , io_outgauge_ip           ///< Config: STR   OutGauge IP
MK_GVAR_DATA(Int        , io_outgauge_port         ///< Config: INT   OutGauge Port
MK_GVAR_DATA(Float      , io_outgauge_delay        ///< Config: FLOAT OutGauge Delay
MK_GVAR_DATA(Int        , io_outgauge_id           ///< Config: INT   OutGauge ID

// Audio
MK_GVAR_DATA(Float      ,audio_master_volume      ///< Config: FLOAT Sound Volume
MK_GVAR_DATA(Bool       ,audio_enable_creak       ///< Config: BOOL  Creak Sound
MK_GVAR_DATA(Str        ,audio_device_name        ///< Config: STR   AudioDevice
MK_GVAR_DATA(Bool       ,audio_menu_music         ///< Config: BOOL  MainMenuMusic 

// Graphics
MK_GVAR_ENUM(GfxFlaresMode  , gfx_flares_mode      "Lights"               , GFX_FLARES_ALL_VEHICLES_HEAD_ONLY , GfxFlaresModeToString );
MK_GVAR_ENUM(GfxShadowType  , gfx_shadow_type      "Shadow technique"     , GFX_SHADOW_TYPE_PSSM              , GfxShadowModeToStr );
MK_GVAR_ENUM(GfxExtCamMode  , gfx_extcam_mode      "External Camera Mode" , GFX_EXTCAM_MODE_PITCHING          ,  );
MK_GVAR_ENUM(GfxSkyMode     , gfx_sky_mode         "Sky effects"          , GFX_SKY_SANDSTORM                 , GfxSkyModeToString );
MK_GVAR_ENUM(GfxTexFilter   , gfx_texture_filter   "Texture Filtering"    , GFX_TEXFILTER_TRILINEAR           , GfxTexFilterToStr );
MK_GVAR_ENUM(GfxVegetation  , gfx_vegetation_mode  "Vegetation"           , GFX_VEGETATION_NONE               , GfxVegetationModeToStr );
MK_GVAR_ENUM(GfxWaterMode   , gfx_water_mode       "Water effects"        , GFX_WATER_BASIC                   , GfxWaterModeToString );


MK_GVAR_DATA(Bool       ,gfx_enable_sunburn        ///< Config: BOOL  Sunburn
MK_GVAR_DATA(Bool       ,gfx_water_waves           ///< Config: BOOL  Waves
MK_GVAR_DATA(Bool       ,gfx_minimap_disabled      ///< Config: BOOL  disableOverViewMap
MK_GVAR_DATA(Int        ,gfx_particles_mode        ///< Config: BOOL  Particles
MK_GVAR_DATA(Bool       ,gfx_enable_glow           ///< Config: BOOL  Glow
MK_GVAR_DATA(Bool       ,gfx_enable_hdr            ///< Config: BOOL  HDR
MK_GVAR_DATA(Bool       ,gfx_enable_heathaze       ///< Config: BOOL  HeatHaze
MK_GVAR_DATA(Bool       ,gfx_envmap_enabled        ///< Config: BOOL  Envmap
MK_GVAR_DATA(Int        ,gfx_envmap_rate           ///< Config: INT   EnvmapUpdateRate
MK_GVAR_DATA(Int        ,gfx_skidmarks_mode        ///< Config: BOOL  Skidmarks
MK_GVAR_DATA(Float      ,gfx_sight_range           ///< Config: FLOAT SightRange
MK_GVAR_DATA(Float      ,gfx_fov_external          ///< Config: FLOAT FOV External
MK_GVAR_DATA(Float      ,gfx_fov_internal          ///< Config: FLOAT FOV Internal
MK_GVAR_DATA(Int        ,gfx_fps_limit             ///< Config: INT   FPS-Limiter

// ================================================================================
// Access functions
// ================================================================================


// Helpers (forward decl.)
typedef const char* (*EnumToStringFn)(int);



void SetVarStr      (std::string&     var, const char* var_name, STR_CREF        new_value);
void SetVarInt      (int&             var, const char* var_name, int             new_value);
void SetVarEnum     (int&             var, const char* var_name, int             new_value,   EnumToStringFn to_str_fn );
void SetVarBool     (bool&            var, const char* var_name, bool            new_value);
void SetVarFloat    (float&           var, const char* var_name, float           new_value);

// Getters


// Setters
void SetActiveAppState    (State    v) { SetVarEnum    (g_app_state_active     , "app_state_active"     , (int)v, AppStateToStr); }
void SetPendingAppState   (State    v) { SetVarEnum    (g_app_state_pending    , "app_state_pending"    , (int)v, AppStateToStr); }
void SetSimActiveTerrain  (STR_CREF v) { SetVarStr     (g_sim_active_terrain   , "sim_active_terrain"   , v); }
void SetSimNextTerrain    (STR_CREF v) { SetVarStr     (g_sim_next_terrain     , "sim_next_terrain"     , v); }
void SetActiveSimState    (SimState v) { SetVarEnum    (g_sim_state_active     , "sim_state_active"     , (int)v, SimStateToStr); }
void SetPendingSimState   (SimState v) { SetVarEnum    (g_sim_state_pending    , "sim_state_pending"    , (int)v, SimStateToStr); }
void SetActiveMpState     (MpState  v) { SetVarEnum    (g_mp_state_active      , "mp_state_active"      , (int)v, MpStateToStr ); }
void SetPendingMpState    (MpState  v) { SetVarEnum    (g_mp_state_pending     , "mp_state_pending"     , (int)v, MpStateToStr ); }
void SetMpServerHost      (STR_CREF v) { SetVarStr     (g_mp_server_host       , "mp_server_host"       , v); }
void SetMpServerPassword  (STR_CREF v) { SetVarStr     (g_mp_server_password   , "mp_server_password"   , v); }
void SetMpServerPort      (int      v) { SetVarInt     (g_mp_server_port       , "mp_server_port"       , v); }
void SetMpPlayerName      (STR_CREF v) { SetVarStr     (g_mp_player_name       , "mp_player_name"       , v); }
void SetDiagTraceGlobals  (bool     v) { SetVarBool    (g_diag_trace_globals   , "diag_trace_globals"   , v); }
void SetSysProcessDir     (STR_CREF v) { SetVarStr     (g_sys_process_dir      , "sys_process_dir"      , v); }
void SetSysUserDir        (STR_CREF v) { SetVarStr     (g_sys_user_dir         , "sys_user_dir"         , v); }
void SetSysConfigDir      (STR_CREF v) { SetVarStr     (g_sys_config_dir       , "sys_config_dir"       , v); }
void SetSysCacheDir       (STR_CREF v) { SetVarStr     (g_sys_cache_dir        , "sys_cache_dir"        , v); }
void SetSysLogsDir        (STR_CREF v) { SetVarStr     (g_sys_logs_dir         , "sys_logs_dir"         , v); }
void SetSysResourcesDir   (STR_CREF v) { SetVarStr     (g_sys_resources_dir    , "sys_resources_dir"    , v); }
void SetIoFFbackEnabled   (bool     v) { SetVarBool    (g_io_ffback_enabled    , "io_ffback_enabled"    , v); }
void SetIoFFbackCameraGain(float    v) { SetVarFloat   (g_io_ffback_camera_gain, "io_ffback_camera_gain", v); }
void SetIoFFbackCenterGain(float    v) { SetVarFloat   (g_io_ffback_center_gain, "io_ffback_center_gain", v); }
void SetIoFFbackMasterGain(float    v) { SetVarFloat   (g_io_ffback_master_gain, "io_ffback_master_gain", v); }
void SetIoFFbackStressGain(float    v) { SetVarFloat   (g_io_ffback_stress_gain, "io_ffback_stress_gain", v); }
void SetGfxShadowType     (GfxShadowType  v) { SetVarEnum    (g_gfx_shadow_type      , "gfx_shadow_mode"      , (int)v, GfxShadowModeToStr); }
void SetGfxExternCamMode  (GfxExtCamMode  v) { SetVarInt     (g_gfx_extcam_mode      , "gfx_extcam_mode"      , (int)v); }
void SetGfxTexFiltering   (GfxTexFilter   v) { SetVarEnum    (g_gfx_texture_filter   , "gfx_texture_filter"   , (int)v, GfxTexFilterToStr ); }
void SetGfxVegetationMode (GfxVegetation  v) { SetVarEnum    (g_gfx_vegetation_mode  , "gfx_vegetation_mode"  , (int)v, GfxVegetationModeToStr); }
void SetGfxEnableSunburn  (bool           v) { SetVarBool    (g_gfx_enable_sunburn   , "gfx_enable_sunburn"   , v); }
void SetGfxWaterUseWaves  (bool           v) { SetVarBool    (g_gfx_water_waves      , "gfx_water_waves"      , v); }
void SetGfxEnableGlow     (bool           v) { SetVarBool    (g_gfx_enable_glow      , "gfx_enable_glow"      , v); }
void SetGfxEnableHdr      (bool           v) { SetVarBool    (g_gfx_enable_hdr       , "gfx_enable_hdr"       , v); }
void SetGfxUseHeathaze    (bool           v) { SetVarBool    (g_gfx_enable_heathaze  , "gfx_enable_heathaze"  , v); }
void SetGfxEnvmapEnabled  (bool           v) { SetVarBool    (g_gfx_envmap_enabled   , "gfx_envmap_enabled"   , v); }
void SetGfxEnvmapRate     (int            v) { SetVarInt     (g_gfx_envmap_rate      , "gfx_envmap_rate"      , v); }
void SetGfxSkidmarksMode  (int            v) { SetVarInt     (g_gfx_skidmarks_mode   , "gfx_skidmarks_mode"   , v); }
void SetGfxParticlesMode  (int            v) { SetVarInt     (g_gfx_particles_mode   , "gfx_particles_mode"   , v); }
void SetGfxMinimapDisabled   (bool        v) { SetVarBool    (g_gfx_minimap_disabled , "gfx_minimap_disabled" , v); }
void SetDiagRigLogNodeImport (bool        v) { SetVarBool    (g_diag_rig_log_node_import  , "diag_rig_log_node_import"  , v); }
void SetDiagRigLogNodeStats  (bool        v) { SetVarBool    (g_diag_rig_log_node_stats   , "diag_rig_log_node_stats"   , v); }
void SetDiagRigLogMessages   (bool        v) { SetVarBool    (g_diag_rig_log_messages     , "diag_rig_log_messages"     , v); }
void SetDiagCollisions       (bool        v) { SetVarBool    (g_diag_collisions           , "diag_collisions"           , v); }
void SetDiagTruckMass        (bool        v) { SetVarBool    (g_diag_truck_mass           , "diag_truck_mass"           , v); }
void SetDiagEnvmap           (bool        v) { SetVarBool    (g_diag_envmap               , "diag_envmap"               , v); }
void SetAppLanguage          (STR_CREF    v) { SetVarStr     (g_app_language              , "app_language"              , v); }
void SetAppLocale            (STR_CREF    v) { SetVarStr     (g_app_locale                , "app_locale"                , v); }
void SetAppMultithread       (bool        v) { SetVarBool    (g_app_multithread           , "app_multithread"           , v); }
void SetAppScreenshotFormat  (STR_CREF    v) { SetVarStr     (g_app_screenshot_format     , "app_screenshot_format"     , v); }
void SetIoInputGrabMode      (IoInputGrabMode v) { SetVarEnum(g_io_input_grab_mode        , "io_input_grab_mode",    (int)v, IoInputGrabModeToStr); }
void SetIoArcadeControls     (bool        v) { SetVarBool    (g_io_arcade_controls        , "io_arcade_controls"        , v); }
void SetAudioMasterVolume  (float         v) { SetVarFloat   (g_audio_master_volume       , "audio_master_volume"       , v); }
void SetAudioEnableCreak   (bool          v) { SetVarBool    (g_audio_enable_creak        , "audio_enable_creak"        , v); }
void SetAudioDeviceName    (STR_CREF      v) { SetVarStr     (g_audio_device_name         , "audio_device_name"         , v); }
void SetAudioMenuMusic     (bool          v) { SetVarBool    (g_audio_menu_music          , "audio_menu_music"          , v); }
void SetSimReplayEnabled   (bool          v) { SetVarBool    (g_sim_replay_enabled        , "sim_replay_enabled"        , v); }
void SetSimReplayLength    (int           v) { SetVarInt     (g_sim_replay_length         , "sim_replay_length"         , v); }
void SetSimReplayStepping  (int           v) { SetVarInt     (g_sim_replay_stepping       , "sim_replay_stepping"       , v); }
void SetSimPositionStorage (bool          v) { SetVarBool    (g_sim_position_storage      , "sim_position_storage"      , v); }
void SetSimNextVehicle     (STR_CREF      v) { SetVarStr     (g_sim_next_vehicle          , "sim_next_vehicle"          , v); }
void SetSimNextVehConfig   (STR_CREF      v) { SetVarStr     (g_sim_next_veh_config       , "sim_next_veh_config"       , v); }
void SetSimNextVehEnter    (bool          v) { SetVarBool    (g_sim_next_veh_enter        , "sim_next_veh_enter"        , v); }
void SetSimGearboxMode     (SimGearboxMode v){ SetVarEnum    (g_sim_gearbox_mode          , "sim_gearbox_mode",      (int)v, SimGearboxModeToString); }
void SetGfxFlaresMode      (GfxFlaresMode v) { SetVarEnum    (g_gfx_flares_mode           , "gfx_flares_mode",       (int)v, GfxFlaresModeToString ); }
void SetSysScreenshotDir   (STR_CREF      v) { SetVarStr     (g_sys_screenshot_dir        , "sys_screenshot_dir"        , v); }
void SetIoOutGaugeMode     (int           v) { SetVarInt     (g_io_outgauge_mode          , "io_outgauge_mode"          , v); }
void SetIoOutGaugeIp       (STR_CREF      v) { SetVarStr     (g_io_outgauge_ip            , "io_outgauge_ip"            , v); }
void SetIoOutGaugePort     (int           v) { SetVarInt     (g_io_outgauge_port          , "io_outgauge_port"          , v); }
void SetIoOutGaugeDelay    (float         v) { SetVarFloat   (g_io_outgauge_delay         , "io_outgauge_delay"         , v); }
void SetIoOutGaugeId       (int           v) { SetVarInt     (g_io_outgauge_id            , "io_outgauge_id"            , v); }
void SetGfxSkyMode         (GfxSkyMode    v) { SetVarEnum    (g_gfx_sky_mode              , "gfx_sky_mode",          (int)v, GfxSkyModeToString   ); }
void SetGfxWaterMode       (GfxWaterMode  v) { SetVarEnum    (g_gfx_water_mode            , "gfx_water_mode",        (int)v, GfxWaterModeToString ); }
void SetGfxSightRange      (float         v) { SetVarFloat   (g_gfx_sight_range           , "gfx_sight_range"           , v); }
void SetGfxFovExternal     (float         v) { SetVarFloat   (g_gfx_fov_external          , "gfx_fov_external"          , v); }
void SetGfxFovInternal     (float         v) { SetVarFloat   (g_gfx_fov_internal          , "gfx_fov_internal"          , v); }
void SetGfxFpsLimit        (int           v) { SetVarInt     (g_gfx_fps_limit             , "gfx_fps_limit"             , v); }
void SetDiagVideoCameras   (bool          v) { SetVarBool    (g_diag_videocameras         , "diag_videocamera"          , v); }

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

void Init()
{
    g_app_state_active     = APP_STATE_BOOTSTRAP;
    g_app_state_pending    = APP_STATE_MAIN_MENU;
    g_app_language         = "English";
    g_app_locale           = "en";
    g_app_screenshot_format= "jpg";
	g_app_multithread      = true;

    g_mp_state_active      = MP_STATE_DISABLED;
    g_mp_state_pending     = MP_STATE_NONE;
    g_mp_player_name       = "Anonymous";



    g_diag_trace_globals   = false; // Don't init to 'true', logger is not ready at startup.


    g_gfx_sight_range      = 3000.f; // Previously either 2000 or 4500 (inconsistent)
    g_gfx_fov_external     = 60.f;
    g_gfx_fov_internal     = 75.f;
    g_gfx_fps_limit        = 0; // Unlimited

    g_io_outgauge_ip       = "192.168.1.100";
    g_io_outgauge_port     = 1337;
    g_io_outgauge_delay    = 10.f;
    g_io_outgauge_mode     = 0; // 0 = disabled, 1 = enabled
    g_io_outgauge_id       = 0;
}


// ================================================================================
// Private helper functions
// ================================================================================


void LogVarUpdate(const char* old_value, const char* new_value)
{

        char log[1000] = "";
       // snprintf(log, 1000, "[RoR|Globals] Updating \"%s\": [%s] => [%s]", name, old_value, new_value);
        LOG(log);
    
}

void SetVarStr (std::string& var, const char* var_name, std::string const & new_value)
{
    //LogVarUpdate(var_name, var.c_str(), new_value.c_str());
    var = new_value;
}

void SetVarInt (int& var, const char* var_name, int new_value)
{
    if (g_diag_trace_globals && (var != new_value))
    {
        char log[1000] = "";
        snprintf(log, 1000, "[RoR|Globals] Updating \"%s\": [%d] => [%d]", var_name, var, new_value);
        LOG(log);
    }
    var = new_value;
}

void SetVarEnum (int& var, const char* var_name, int new_value, EnumToStringFn enum_to_str_fn)
{
    //LogVarUpdate(var_name, (*enum_to_str_fn)(var), (*enum_to_str_fn)(new_value));
    var = new_value;
}

void SetVarBool (bool& var, const char* var_name, bool new_value)
{
    //LogVarUpdate(var_name, (var ? "True" : "False"), (new_value ? "True" : "False"));
    var = new_value;
}

void SetVarFloat(float& var, const char* var_name, float new_value)
{
    if (g_diag_trace_globals && (var != new_value))
    {
        char log[1000] = "";
        snprintf(log, 1000, "[RoR|Globals] Updating \"%s\": [%f] => [%f]", var_name, var, new_value);
        LOG(log);
    }
    var = new_value;
}

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

/*
    This source file is part of Rigs of Rods
    Copyright 2016 Petr Ohlidal

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

// ######################################################################### //
//                             Project NextSim                               //
// ######################################################################### //

#include "Simulation.h"

#include "Application.h"
#include "ContentManager.h"
#include "InputEngine.h"
#include "OgreScriptBuilder.h"
#include "OgreSubsystem.h"
#include "GUIManager.h"
#include "ScriptEngine.h"
#include "ThreadPool.h"

#include <limits>
#include <OgreLogManager.h>
#include <OgreManualObject.h>
#include <OgreMaterial.h>
#include <OgreRenderWindow.h>
#include <OgreResourceGroupManager.h>
#include <OgreRoot.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <scriptarray/scriptarray.h>
#include <scriptstdstring/scriptstdstring.h>
#include <scriptmath/scriptmath.h>
#include <scriptany/scriptany.h>
#include "OgreAngelscript.h"

using namespace RoR;

#define AS_SimPrepare_FN_PROTOTYPE "bool SimPrepare(SimContext@ ctx)"
#define AS_SimUpdate_FN_PROTOTYPE  "bool SimUpdate(SimContext@ ctx, uint dt_milis)"
#define AS_SimCleanup_FN_PROTOTYPE "void SimCleanup()"

#define LOGSTREAM Ogre::LogManager::getSingleton().stream() << "[RoR|Sim] "
#define SCRIPTLOG Ogre::LogManager::getSingleton().stream() << "[RoR|Scripting] "

void AsLogMessage(std::string msg)
{
    Ogre::LogManager::getSingleton().stream() << "[RoR|ScriptLog] " << msg;
}

// ============================= GfxContext ================================ //

GfxContext::GfxContext():
    m_scene_mgr(nullptr),
    m_camera(nullptr),
    m_viewport(nullptr),
    m_is_initialized(false)
{
    m_data.camera_pos = Ogre::Vector3::ZERO;
    m_data.camera_rot = Ogre::Quaternion::IDENTITY;
}

void GfxContext::CheckAndInit()
{
    if (m_is_initialized)
        return; // Already initialized

    App::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::TEXTURES); // Skybox (dds)
    App::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::MATERIALS); // RoR default skybox material
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

    m_scene_mgr = App::GetOgreSubsystem()->GetOgreRoot()->createSceneManager(Ogre::ST_GENERIC, "sim2_scene_manager");
    m_scene_mgr->setAmbientLight(Ogre::ColourValue::White);

    m_camera = m_scene_mgr->createCamera("sim2_camera");
    m_camera->setAutoAspectRatio(true);
    m_camera->setNearClipDistance(0.1f);
    m_camera->setFarClipDistance(App::GetGfxSightRange());
    m_camera->setFOVy(Ogre::Degree(App::GetGfxFovExternal()));
    m_is_initialized = true;
}

void GfxContext::CopyLogicData(LogicContext* logic_ctx)
{
    // ## Done on main thread, once per loop, before tasks are put to threadpool
    // ## Must be quick: just copy data, no processing (that's the job of gfx task!)

    // Camera
    m_data.camera_pos        = logic_ctx->GetCamPos();
    m_data.camera_rot        = logic_ctx->GetCamRot();

    // Actors
    m_data.actors_added.clear();
    m_data.actors_removed.clear();
    for (ActorLogic& actor_logic: logic_ctx->GetActors())
    {
        if (actor_logic.state == ActorLogic::STATE_ADDED)
        {
            m_data.actors_added.push_back(actor_logic.actor); // Doesn't have gfx state object yet
        }
        else if (actor_logic.state == ActorLogic::STATE_REMOVING)
        {
            m_data.actors_removed.push_back(actor_logic.actor); // Not updated anymore; will be deleted in next loop
        }
        else
        {
            NodeLogic*     src_nodes    = actor_logic.nodes.get();
            size_t         num_nodes    = actor_logic.actor->def->nodes.size();
            Ogre::Vector3* dst_node_pos = actor_logic.actor->gfx->node_positions.get();

            for (size_t i = 0; i < num_nodes; ++i)
            {
                dst_node_pos[i] = src_nodes[i].pos;
            }
        }
    }
}

bool GfxContext::Prepare()
{
    try
    {
        this->CheckAndInit();

        m_viewport = RoR::App::GetOgreSubsystem()->GetViewport();
        int viewport_width = m_viewport->getActualWidth();
        m_viewport->setBackgroundColour(Ogre::ColourValue::Black);
        m_camera->setAspectRatio(m_viewport->getActualHeight() / viewport_width);
        m_viewport->setCamera(m_camera);

        // Demo scene - ground
        Ogre::Plane ground_plane(Ogre::Vector3::UNIT_Y, 0);
        m_ground_mesh = Ogre::MeshManager::getSingleton().createPlane(
            "sim2_ground_mesh",
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            ground_plane,
            1500, 1500, 20, 20,
            true,
            1, 5, 5,
            Ogre::Vector3::UNIT_Z);

        m_ground_node = m_scene_mgr->getRootSceneNode()->createChildSceneNode();
        m_ground_entity = m_scene_mgr->createEntity(m_ground_mesh);
        m_ground_entity->setMaterialName("asphalt"); // Built-in managed material
        m_ground_node->attachObject(m_ground_entity);

        // Demo scene - sky
        m_scene_mgr->setSkyBox(true, "tracks/skyboxcol", 100, true); // RoR default sky box

        return true;
    }
    catch (Ogre::Exception e)
    {
        LOGSTREAM << "Fatal error in " <<__FUNCTION__ << ", message: " << e.getFullDescription();
        return false;
    }
}

void GfxContext::Update()
{
    // Camera
    m_camera->setPosition(m_data.camera_pos);
    m_camera->setOrientation(m_data.camera_rot);

    // Removed actors (not updated anymore)
    for (Actor* removed_actor : m_data.actors_removed)
    {
        auto itor = m_actors.begin();
        auto endi = m_actors.end();
        while ((itor != endi) && (itor->actor != removed_actor))
        {
            ++itor;
        }
        assert(itor != endi); // Illegal state: corresponding ActorGfx not found!
        m_actors.erase(itor);
        removed_actor->gfx = nullptr;
    }

    // Active actors
    for (ActorGfx& actor : m_actors)
    {
        actor.Update();
    }

    // Added actors (not updated yet)
    for (Actor* new_actor : m_data.actors_added)
    {
        m_actors.push_back(ActorGfx(new_actor, m_scene_mgr));
    }

    // Render and update display
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX
    Ogre::WindowEventUtilities::messagePump();
#endif
    App::GetOgreSubsystem()->GetOgreRoot()->renderOneFrame();
}

// =============================== ActorGfx ================================ //

ActorGfx::ActorGfx(Actor* actor, Ogre::SceneManager* scene_mgr)
{
    // ## Simple debug visualization - wireframe

    node_positions = std::unique_ptr<Ogre::Vector3>(new Ogre::Vector3[actor->def->nodes.size()]);
    state = STATE_PREPARING;

    // Initialize meshes
    for (MeshDef& mesh_def : actor->def->meshes)
    {
        // Create manual mesh
        MeshGfx mesh_gfx;
        mesh_gfx.def = &mesh_def;
        mesh_gfx.manual_mesh = scene_mgr->createManualObject();

        // Create material
        static size_t id_source;
        char mat_name[30];
        sprintf(mat_name, "%.20s_%08x", actor->def->name.c_str(), id_source++);
        mesh_gfx.material = Ogre::MaterialManager::getSingleton().create(mat_name, "nextsim_mat");
        mesh_gfx.material->setDiffuse(mesh_def.color);
        mesh_gfx.material->setReceiveShadows(false);
        mesh_gfx.material->setTransparencyCastsShadows(false);

        // Initialize the manual mesh by building it once
        //    (requires slightly different code than updates)
        size_t num_lines = mesh_def.vis_beams.size();
        mesh_gfx.manual_mesh->begin(mat_name);
        const Ogre::Vector3 pos(0.f, std::numeric_limits<float>::min(), 0.f); // Deep underground = out of sight
        for (size_t i = 0; i < num_lines; ++i)
        {
            mesh_gfx.manual_mesh->position(pos);
            mesh_gfx.manual_mesh->position(pos);
        }
        mesh_gfx.manual_mesh->end();

        meshes.push_back(mesh_gfx);
    }
}

void ActorGfx::Update()
{
    // ## Simple debug visualization (wireframe)
    // ## - ActorGfx knows all node positions (copied from ActorLogic)
    // ## - MeshDef specifies which node pairs should be visualized

    Ogre::Vector3* nodepos_raw = node_positions.get();
    for (MeshGfx& mesh_gfx : meshes)
    {
        auto& line_defs = mesh_gfx.def->vis_beams;
        mesh_gfx.manual_mesh->beginUpdate(0);
        for (auto& line_def : line_defs)
        {
            mesh_gfx.manual_mesh->position(nodepos_raw[line_def.first]);
            mesh_gfx.manual_mesh->position(nodepos_raw[line_def.second]);
        }
        mesh_gfx.manual_mesh->end();
    }
}

// ============================= LogicContext ============================== //

LogicContext::LogicContext():
    m_cur_buffer_idx(0),
    m_prev_buffer_idx(1),
    m_script_engine(nullptr),
    m_script_context(nullptr),
    m_script_setup_fn(nullptr),
    m_script_update_fn(nullptr),
    m_script_cleanup_fn(nullptr),
    m_is_initialized(false)
{
    this->Reset();
}

void LogicContext::Reset()
{
    m_camera_pos          = Ogre::Vector3::ZERO;
    m_camera_rot          = Ogre::Quaternion::IDENTITY;
    m_keyboard_changed    = false;
    m_mouse_changed       = false;
    m_exit_requested      = false;
    m_cur_buffer_idx      = 0;
    m_prev_buffer_idx     = 1;

    memset(m_key_states,  0, sizeof(m_key_states));
    memset(m_mouse_state, 0, sizeof(m_mouse_state));
}

bool LogicContext::Prepare()
{
    this->Reset();
    App::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::SCRIPTS);

    if (! this->CheckAndInit())
        return false;

    int res = m_script_context->Prepare(m_script_setup_fn);
    if (res != 0)
    {
        m_err_msg << "Failed to setup context for 'SimPrepare': " << AsRetCodeToString(res);
        return false;
    }

    res = m_script_context->SetArgObject(0, static_cast<void*>(this));
    if (res != 0)
    {
        m_err_msg << "Failed to pass argument 0 'ctx' for 'SimPrepare()': " << AsRetCodeToString(res);
        return false;
    }

    std::string err_msg;
    if (! AsExecuteContext(m_script_context, m_script_engine, err_msg))
    {
        m_err_msg << err_msg;
        return false;
    }

    if (m_script_context->GetReturnDWord() == 0) // 'SimSetup()' returned false => error
    {
        m_err_msg << "Failure in script function 'SimSetup()'";
        return false;
    }

    return true;
}

int AsIncludeCallback(const char *include, const char *from, AngelScript::CScriptBuilder *builder_ptr, void *userParam)
{
    auto* builder = static_cast<OgreScriptBuilder*>(builder_ptr);
    return builder->AddSectionFromFile(include);
}

#define ON_ERROR_RETURN(_HELPER_)         \
    if (_HELPER_.CheckError())            \
    {                                     \
        LOGSTREAM << _HELPER_.GetError(); \
        return false;                     \
    }

bool LogicContext::CheckAndInit()
{
    if (m_is_initialized)
        return true; // Already done

    m_script_engine = AngelScript::asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if (m_script_engine == nullptr)
    {
        LOGSTREAM << "Failed to create scripting engine";
        return false;
    }

    int res = m_script_engine->SetMessageCallback(
        AngelScript::asMETHOD(LogicContext,ScriptMsgCallback), this, AngelScript::asCALL_THISCALL);
    if (res != 0)
    {
        SCRIPTLOG << "failed to register message callback, error: " << AsRetCodeToString(res);
        return false;
    }

    AngelScript::RegisterScriptArray     (m_script_engine, true);
    AngelScript::RegisterStdString       (m_script_engine);
    AngelScript::RegisterStdStringUtils  (m_script_engine);
    AngelScript::RegisterScriptMath      (m_script_engine);
    AngelScript::RegisterScriptAny       (m_script_engine);
    AngelScript::RegisterScriptDictionary(m_script_engine);

    AsSetupHelper helper(m_script_engine);

    RegisterOgreObjects(&helper);

    helper.RegisterGlobalFn("void LogMessage(string)", AngelScript::asFUNCTION(AsLogMessage), AngelScript::asCALL_CDECL);

    AsObjectRegProxy obj(&helper, "SimContext", 0, AngelScript::asOBJ_REF);
    obj.AddBehavior(AngelScript::asBEHAVE_ADDREF,  "void f()", AngelScript::asMETHOD(LogicContext, DummyAddRef),     AngelScript::asCALL_THISCALL);
    obj.AddBehavior(AngelScript::asBEHAVE_RELEASE, "void f()", AngelScript::asMETHOD(LogicContext, DummyReleaseRef), AngelScript::asCALL_THISCALL);

    obj.AddMethod("void Quit()",                               AngelScript::asMETHOD(LogicContext, Quit));
    obj.AddMethod("bool IsKeyDown(int keycode)",               AngelScript::asMETHOD(LogicContext, IsKeyDown));
    obj.AddMethod("bool HasKbChanged()",                       AngelScript::asMETHOD(LogicContext, HasKbChanged));
    obj.AddMethod("bool WasKeyPressed(int keycode)",           AngelScript::asMETHOD(LogicContext, WasKeyPressed));
    obj.AddMethod("bool WasKeyReleased()",                     AngelScript::asMETHOD(LogicContext, WasKeyReleased));
    obj.AddMethod("void SetCameraPosition(Vector3 pos)",       AngelScript::asMETHOD(LogicContext, SetCameraPosition));
    obj.AddMethod("void SetCameraOrientation(Quaternion rot)", AngelScript::asMETHOD(LogicContext, SetCameraOrientation));

    if (helper.CheckErrors())
    {
        LOGSTREAM << "Errors while registering LogicContext interface, messages:\n" << helper.GetErrors();
        return false;
    }

    OgreScriptBuilder builder;
    builder.SetResourceGroup("PackedScripts");
    builder.SetIncludeCallback(AsIncludeCallback, nullptr);

    res = builder.StartNewModule(m_script_engine, "sim");
    if (res != 0)
    {
        SCRIPTLOG << "error creating module 'main': " << AsRetCodeToString(res);
        return false;
    }

    res = builder.AddSectionFromFile("SimMain.as");
    if (res != 0)
    {
        SCRIPTLOG << "error loading script 'SimMain.as': " << AsRetCodeToString(res);
        return false;
    }

    res = builder.BuildModule();
    if (res != 0)
    {
        SCRIPTLOG << "error bulding module: " << AsRetCodeToString(res);
        return false;
    }

    AngelScript::asIScriptModule* main_module = m_script_engine->GetModule("sim");
    m_script_setup_fn   = main_module->GetFunctionByDecl(AS_SimPrepare_FN_PROTOTYPE);
    m_script_update_fn  = main_module->GetFunctionByDecl(AS_SimUpdate_FN_PROTOTYPE);
    m_script_cleanup_fn = main_module->GetFunctionByDecl(AS_SimCleanup_FN_PROTOTYPE);
    if (!m_script_setup_fn || !m_script_update_fn || !m_script_cleanup_fn)
    {
        SCRIPTLOG << "Failed to find function(s): "
            << (m_script_setup_fn   ? "" : AS_SimPrepare_FN_PROTOTYPE) << " "
            << (m_script_update_fn  ? "" : AS_SimUpdate_FN_PROTOTYPE) << " "
            << (m_script_cleanup_fn ? "" : AS_SimCleanup_FN_PROTOTYPE);
        return false;
    }

    m_script_context = m_script_engine->CreateContext();
    if (m_script_context == nullptr)
    {
        SCRIPTLOG << "Failed to create script context";
        return false;
    }

    m_is_initialized = true;
    return true;
}

void LogicContext::UpdateUserInput(OIS::Keyboard* ois_kb, OIS::Mouse* ois_mouse)
{
    // Keyboard
    ois_kb->capture();
    ois_kb->copyKeyStates(this->GetCurKeyStates());
    m_keyboard_changed = memcmp(m_key_states[0], m_key_states[1], KEY_STATES_LEN) != 0;

    // Mouse
    ois_mouse->capture();
    MouseState& mstate = this->GetCurMouseState();
    OIS::MouseState const& ois_mstate = ois_mouse->getMouseState();
    mstate.x_abs   = ois_mstate.X.abs;
    mstate.x_rel   = ois_mstate.X.rel;
    mstate.y_abs   = ois_mstate.Y.abs;
    mstate.y_rel   = ois_mstate.Y.rel;
    mstate.z_rel   = ois_mstate.Z.rel; // Mousewheel doesn't have absolute position :)
    mstate.buttons = ois_mstate.buttons;
    m_mouse_changed = memcmp(&m_mouse_state[0], &m_mouse_state[1], sizeof(MouseState)) != 0;
}

bool LogicContext::Update(size_t dt_milis)
{
    int res = m_script_context->Prepare(m_script_update_fn);
    if (res != 0)
    {
        m_err_msg << "[RoR|Script] Failed to setup context for 'SimUpdate': " << AsRetCodeToString(res);
        return false;
    }

    res = m_script_context->SetArgObject(0, static_cast<void*>(this));
    if (res != 0)
    {
        m_err_msg << "[RoR|Script] Failed to pass argument 0 'ctx' for 'SimUpdate()': " << AsRetCodeToString(res);
        return false;
    }

    res = m_script_context->SetArgDWord(1, AngelScript::asUINT(dt_milis));
    if (res != 0)
    {
        m_err_msg << "[RoR|Script] Failed to pass argument 1 'dt_milis' (" << dt_milis << ") for 'SimUpdate()': " << AsRetCodeToString(res);
        return false;
    }

    std::string err_msg;
    if (! AsExecuteContext(m_script_context, m_script_engine, err_msg))
    {
        m_err_msg << err_msg;
        return false;
    }

    if (m_script_context->GetReturnDWord() == 0) // 'SimUpdate()' returned false => we should exit
    {
        return false; // No err message = clean exit
    }

    return true;
}

void LogicContext::ScriptMsgCallback(const AngelScript::asSMessageInfo *msg)
{
    char tmp[1000];
    snprintf(tmp, 1000, "%s: %s (section %s, line %d, char %d)",
        AsMsgTypeToString(msg->type), msg->message, msg->section, msg->row, msg->col);
    SCRIPTLOG << tmp;
}

void LogicContext::Cleanup()
{
    int res = m_script_context->Prepare(m_script_cleanup_fn);
    if (res != 0)
    {
        m_err_msg << "[RoR|Script] Failed to setup context for 'SimCleanup': " << AsRetCodeToString(res);
        return;
    }

    std::string err_msg;
    if (! AsExecuteContext(m_script_context, m_script_engine, err_msg))
    {
        m_err_msg << err_msg;
    }
}

#define VALID_OR_RETURN(_KC_, _NAME_)                            \
    if (_KC_ < 0 || _KC_ > KEY_STATES_LEN)                       \
    {                                                            \
        SCRIPTLOG << "WARNING! Script called '" << _NAME_        \
            << "' with invalid argument: [" << keycode << "]";   \
        return false;                                            \
    }

bool LogicContext::IsKeyDown(int keycode)
{
    VALID_OR_RETURN(keycode, "IsKeyDown()");

    return m_key_states[m_cur_buffer_idx][keycode] == 1;
}

bool LogicContext::WasKeyPressed(int keycode)
{
    VALID_OR_RETURN(keycode, "WasKeyPressed()");

    return (m_key_states[m_cur_buffer_idx ][keycode] == 1)
        && (m_key_states[m_prev_buffer_idx][keycode] == 0);
}

bool LogicContext::WasKeyReleased(int keycode)
{
    VALID_OR_RETURN(keycode, "WasKeyReleased()");

    return (m_key_states[m_cur_buffer_idx ][keycode] == 0)
        && (m_key_states[m_prev_buffer_idx][keycode] == 1);
}

void LogicContext::SetCameraPosition (Ogre::Vector3 pos)
{
    m_camera_pos = pos;
}

void LogicContext::SetCameraOrientation (Ogre::Quaternion orientation)
{
    m_camera_rot = orientation;
}

void LogicContext::StartNewFrame()
{
    m_cur_buffer_idx = (m_cur_buffer_idx == 0) ? 1 : 0;
    m_prev_buffer_idx = (m_prev_buffer_idx == 0) ? 1 : 0;
}

// ============================== Simulation =============================== //

Simulation::Simulation():
    m_keyboard(nullptr),
    m_mouse(nullptr)
{
}

bool Simulation::Prepare()
{
    if (m_keyboard == nullptr)
        m_keyboard = App::GetInputEngine()->GetOisKeyboard();

    if (m_mouse == nullptr)
        m_mouse = App::GetInputEngine()->GetOisMouse();

    if (! m_gfx_context.Prepare())
    {
        LOGSTREAM << "Error preparing GfxContext for simulation";
        return false;
    }

    if (! m_logic_context.Prepare())
    {
        LOGSTREAM << "Error preparing LogicContext for simulation: " << m_logic_context.GetError();
        return false;
    }

    return true;
}

void Simulation::EnterLoop()
{
    // Setup application
    App::GetInputEngine()->SetKeyboardListener(nullptr);
    App::GetInputEngine()->SetMouseListener(nullptr);
    App::GetOgreSubsystem()->GetOgreRoot()->removeFrameListener(App::GetGuiManager()); // Stop GUIManager updates

    ThreadPool threadpool(1); // Just background logic thread for now...

    Ogre::Timer timer;
    size_t time_ms_last;
    size_t time_ms_cur = timer.getMilliseconds();
    while (! m_logic_context.WasExitRequested())
    {
        // Update time
        time_ms_last = time_ms_cur;
        time_ms_cur = timer.getMilliseconds();
        size_t dt_milis = time_ms_cur - time_ms_last;

        // Copy logic data for gfx processing
        m_gfx_context.CopyLogicData(&m_logic_context);

        // Update inputs
        m_logic_context.StartNewFrame();
        m_logic_context.UpdateUserInput(m_keyboard, m_mouse);

        // Run logic
        std::shared_ptr<Task> logic_task = threadpool.RunTask(
            [this, dt_milis]{ this->m_logic_context.Update(dt_milis); });

        // Update gfx on main thread.
        // OGRE engine 1.x must render on the thread which initialized it.
        this->m_gfx_context.Update();

        // Wait for logic
        logic_task->join();
    }

    // Cleanup
    m_logic_context.Cleanup();

    // Restore application
    App::GetInputEngine()->RestoreKeyboardListener();
    App::GetInputEngine()->RestoreMouseListener();
    App::GetOgreSubsystem()->GetOgreRoot()->addFrameListener(App::GetGuiManager()); // Restore GUIManager updates
}


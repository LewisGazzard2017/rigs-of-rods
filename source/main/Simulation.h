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

#pragma once

// ######################################################################### //
//                             Project NextSim                               //
// ######################################################################### //

/// @file
/// @author Petr Ohlidal
/// @date 11/2016
/// @brief The simulation loop
///
/// TERMINOLOGY:
///   Actor = softbody entity of any kind/purpose/function.
///   Node = softbody vertex(point) = infinitely small perfect ball joint with mass.
///   Beam = softbody edge between 2 nodes = damped spring.
///
/// ARCHITECTURE:
///   A threadpool with 2 task contexts:
///    * LogicContext - manages all game logic/physics data.
///    * GfxContext   - manages 3D scene/gui/rendering.
///
/// LOOP (main thread):
///   1. delta time is measured
///   5. main thread copies state data from LogicContext to GfxContext
///   2. inputs are collected
///   3. logic/gfx tasks are enqueued to threadpool
///        LogicContext: processes current logic frame from user inputs and network.
///        GfxContext: updates scene from prev. logic frame data (copy).
///   4. thredpool waits for all tasks to finish
///
/// NOTES:
///   Communication between LogicContext and GfxContext is one-way; LogicContext
///   updates things at will and doesn't care what GfxContext does with it.
///   Some hints are given, i.e. when actor is removed, it's assigned REMOVED
///   state for GfxContext to process, but there's no feedback.
///
///   Input handling: We copy raw input data from OIS and use our own utilities
///   to access it (OIS constants apply). Reason: limiting reliance on OIS
/// 

#include "ForwardDeclarations.h"

#include <OgreColourValue.h>
#include <OgreMaterial.h>
#include <OgreQuaternion.h>
#include <OgreVector3.h>
#include <list>
#include <memory>
#include <vector>
#include <OISKeyboard.h>
#include <angelscript.h>

namespace RoR {

// Forward
struct ActorLogic; struct ActorGfx; struct ActorDef; struct Actor; class LogicContext; class GfxContext;

// --------------------------------- input --------------------------------- //

struct MouseState
{
    int x_abs, y_abs; ///< Cursor
    int x_rel, y_rel; ///< Cursor
    int z_rel;        ///< Wheel (doesn't have absolute position)
    int buttons;
};

// -------------------------------- actors --------------------------------- //

struct NodeDef
{
    float x, y, z, mass;
};

struct BeamDef
{
    size_t nodes[2];
    float spring, damp;
};

/// Simple debug visualization
struct MeshDef
{
    std::vector<std::pair<size_t, size_t> > vis_beams;
    Ogre::ColourValue color;
};

/// No physics atm, just position
struct NodeLogic
{
    Ogre::Vector3 pos;
};

/// Simple debug visualization
struct MeshGfx
{
    MeshDef* def;
    Ogre::ManualObject* manual_mesh;
    Ogre::MaterialPtr material;
};

struct ActorLogic
{
    enum State
    {
        STATE_UNKNOWN,
        STATE_ADDED,
        STATE_ACTIVE,
        STATE_REMOVING
    };

    std::unique_ptr<NodeLogic> nodes;
    Actor* actor;
    State state;
};

struct ActorGfx
{
    enum State
    {
        STATE_UNKNOWN = 0,
        STATE_PREPARING,     ///< Freshly added to logic context; gfx may not be loaded/ready yet; actor either invisible or in debug view.
        STATE_READY,         ///< All gfx loaded and active
        STATE_HIDDEN,        ///< Hidden on request from logic context
    };

    ActorGfx(Actor* actor, Ogre::SceneManager* scene_mgr);

    void Update();

    std::list<MeshGfx> meshes;
    Actor*             actor;
    State              state;

    std::unique_ptr<Ogre::Vector3> node_positions;
};

/// Read-only definition of an actor (softbody) - both logic/gfx data
struct ActorDef
{
    std::string name;
    std::vector<NodeDef> nodes;
    std::vector<BeamDef> beams;
    std::list<MeshDef> meshes;
};

/// Master actor entry, managed by main thread.
struct Actor
{
    ActorDef*   def;
    ActorLogic* logic; ///< Pointer updated by logic thread(s)!
    ActorGfx*   gfx;   ///< Pointer updated by gfx thread(s)!
};

// ---------------------------------- gfx ---------------------------------- //

class GfxContext
{
public:
    GfxContext();

    bool Prepare();
    bool Cleanup();
    void CopyLogicData(LogicContext* logic_ctx);
    void Update();

protected:
    void CheckAndInit();

    Ogre::SceneManager* m_scene_mgr;
    Ogre::Camera*       m_camera;
    Ogre::Viewport*     m_viewport;
    std::list<ActorGfx> m_actors;
    bool                m_is_initialized;

    Ogre::SceneNode*    m_ground_node;
    Ogre::MeshPtr       m_ground_mesh; ///< Demo
    Ogre::Entity*       m_ground_entity;

    struct
    {
        Ogre::Vector3     camera_pos;
        bool              camera_lookat_set;
        Ogre::Vector3     camera_lookat_pos;
        Ogre::Quaternion  camera_rot;
        std::list<Actor*> actors_added;
        std::list<Actor*> actors_removed;
    }                   m_data;
};

// --------------------------------- logic --------------------------------- //

class LogicContext
{
public:
    static const size_t KEY_STATES_LEN = 256;

    LogicContext();

    bool        Prepare          ();
    bool        Update           (size_t dt_milis);
    void        StartNewFrame    ();
    bool        WasExitRequested () { return m_exit_requested; }
    void        Cleanup          ();
    std::string GetError         () { return m_err_msg.str(); }

    // AngelScript utils
    void DummyAddRef          () {}; ///< For AngelScript to be happy
    void DummyReleaseRef      () {}; ///< For AngelScript to be happy

    // AngelScript interface
    void Quit                 () { m_exit_requested = true; } ///< Exit to main menu
    void ScriptMsgCallback    (const AngelScript::asSMessageInfo* msg);
    bool IsKeyDown            (int keycode);
    bool WasKeyPressed        (int keycode);
    bool WasKeyReleased       (int keycode);
    bool HasKbChanged         () { return m_keyboard_changed; }
    void SetCameraPosition    (Ogre::Vector3 pos);            ///< Only effective in free-look and similar modes.
    void SetCameraOrientation (Ogre::Quaternion orientation); ///< Quaternion; Only effective in free-look and similar modes.

    char*                  GetCurKeyStates()  { return m_key_states[m_cur_buffer_idx]; }
    MouseState&            GetCurMouseState() { return m_mouse_state[m_cur_buffer_idx]; }
    Ogre::Vector3&         GetCamPos()        { return m_camera_pos; }
    Ogre::Quaternion&      GetCamRot()        { return m_camera_rot; }
    std::list<ActorLogic>& GetActors()        { return m_actors; }

    void                   UpdateUserInput(OIS::Keyboard* kb, OIS::Mouse* mouse);

protected:
    bool CheckAndInit();
    void Reset();

    // State
    std::stringstream     m_err_msg;
    bool                  m_exit_requested;
    bool                  m_is_initialized;
    // Input
    size_t                m_cur_buffer_idx; ///< Data buffer index: 0/1
    size_t                m_prev_buffer_idx; ///< Data buffer index: 0/1
    char                  m_key_states[2][KEY_STATES_LEN]; ///< Current (m_cur_buffer_idx) and previous frame.
    MouseState            m_mouse_state[2];                ///< Current (m_cur_buffer_idx) and previous frame.
    bool                  m_keyboard_changed;
    bool                  m_mouse_changed;
    // Simulation
    Ogre::Vector3         m_camera_pos;
    Ogre::Quaternion      m_camera_rot;
    std::list<ActorLogic> m_actors;
    // Scripting
    AngelScript::asIScriptEngine*   m_script_engine;
    AngelScript::asIScriptContext*  m_script_context;
    AngelScript::asIScriptFunction* m_script_setup_fn;
    AngelScript::asIScriptFunction* m_script_update_fn;
    AngelScript::asIScriptFunction* m_script_cleanup_fn;
};

// ------------------------------- simulator ------------------------------- //

class Simulation
{
public:
    Simulation();
    bool Prepare();
    void EnterLoop();

protected:
    GfxContext          m_gfx_context;
    LogicContext        m_logic_context;
    std::list<Actor>    m_actors;
    OIS::Keyboard*      m_keyboard;
    OIS::Mouse*         m_mouse;
};

} // namespace RoR

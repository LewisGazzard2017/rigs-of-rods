/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2016+     Petr Ohlidal & contributors

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

/// @file
/// @brief Implements a player-controlled character (avatar)
/// NOTE: NextSim approach is employed: there are separate game-logic and gfx states.
/// Right now the separation is only half-baked (compatibility), but will help later.

#pragma once

#include "RoRPrerequisites.h"
#include "Network.h"

namespace RoR {

/// 3D engine objects
class CharacterGfx
{
public:
    CharacterGfx();
    void  Load               (Ogre::SceneManager* scene_mgr, const char* obj_name);
    void  Unload             (Ogre::SceneManager* scene_mgr);
    void  AddNetLabel        (const char* obj_name);
    void  UpdatePlayerColor  (int colornum);
    void  PushRotation       (Ogre::Radian rot_horizontal);
    void  PushRotation       (Ogre::Quaternion rot);
    void  PushPosition       (Ogre::Vector3 pos);
    void  PushVisible        (bool vis);
    void  UpdateNetLabel     (const char* caption, Ogre::Vector3 cam_pos, Ogre::Vector3 character_pos);
    float AdvanceAnimation   (const char* anim_name, float time); ///< Returns updated anim time (temporary)
    void  PushSteerAnim      (float steer_angle);
    void  SetNetLabelVisible (bool visible);
    void  SetCastShadows     (bool v);

private:
    void SetActiveAnim     (const char* anim_name);

    Ogre::MaterialPtr        m_material;
    Ogre::Entity*            m_entity;
    Ogre::SceneNode*         m_scene_node;
    Ogre::AnimationStateSet* m_anim_states;
    Ogre::MovableText*       m_net_label;
    const char*              m_active_anim_name;
};

/// Physics and simulation logic
class LocalCharacter
{
public:
    LocalCharacter();
    ~LocalCharacter();
    void                 UpdateNetLabel     (Ogre::Vector3 camera_pos);
    void                 NetSendStreamSetup ();
    void                 NetSendState  ();
    inline void          SetRotation        (Ogre::Radian rot_horizontal) { m_rotation = rot_horizontal; }
    inline void          SetVisible         (bool vis) { m_is_visible = vis; } // TODO: update survey map
    void                 SetPosition        (Ogre::Vector3 pos);
    void                 UnwindMovement     (float distance);
    void                 Update             (float dt_sec); // NOTE: After calling this, must manually update GFX pos/rot and send stream data!
    void                 AttachToTruck      (Beam* truck);
    void                 DetachFromTruck    ();
    inline Ogre::Vector3 GetPosition        () const { return m_position; }
    inline Ogre::Radian  GetRotation        () const { return m_rotation; }
    inline void          SetUsePhysics      (bool v) { m_has_physics = v; }
    inline Beam*         GetCoupledTruck    () { return m_coupled_truck; }

private:
    CharacterGfx         m_gfx; // TODO: Should be managed separately, but compatibility... ~only_a_ptr, 01/2017
    std::string          m_obj_name;
    int                  m_net_source_id;
    int                  m_net_stream_id;
    Beam*                m_coupled_truck;
    const char*          m_active_anim_name;
    float                m_active_anim_time;
    // Positioning
    Ogre::Vector3        m_position;
    Ogre::Radian         m_rotation;
    float                m_speed_h;
    float                m_speed_v;
    std::deque<Ogre::Vector3> m_prev_positions;
    // Bit flags
    bool                 m_is_visible:1;
    bool                 m_has_physics:1;
    bool                 m_can_jump:1;
    bool                 m_is_coupled_visible:1;
};

class RemoteCharacter
{
public:
    RemoteCharacter(int source_id, int stream_id);
    ~RemoteCharacter();
    void        UpdateNetLabel    (Ogre::Vector3 camera_pos);
    void        HandleStreamData  (char* buffer);
    inline int  GetSourceId       () const { return m_net_source_id; }
    inline int  GetStreamId       () const { return m_net_stream_id; }
    void        AttachToTruck     (Beam* truck);
    void        DetachFromTruck   ();
    
private:
    void        ReportError(const char* detail);

    CharacterGfx         m_gfx;
    std::string          m_obj_name;
    int                  m_net_source_id;
    int                  m_net_stream_id;
    Beam*                m_coupled_truck;
    Ogre::Vector3        m_position; ///< Only applies when not coupled with vehicle
};
    
class CharacterFactory
{
public:
    CharacterFactory();
    LocalCharacter*        CreateLocalCharacter (int color_num);
    RemoteCharacter*       CreateRemoteCharacter(int source_id, int stream_id);
    void                   DeleteRemoteCharacter(int source_id);
    void                   DeleteAllCharacters  ();
    void                   HandleStreamData(std::vector<RoR::Networking::recv_packet_t>& packets);
    inline LocalCharacter* GetLocalCharacter() { return m_local_obj; }
    void                   Update(float dt_sec, Ogre::Vector3 camera_pos);
    
private:
    LocalCharacter*               m_local_obj;
    std::vector<RemoteCharacter*> m_remote_objs;
};
    
} // namespace RoR

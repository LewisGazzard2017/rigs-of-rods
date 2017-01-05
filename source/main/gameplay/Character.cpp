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

#include "Character.h"

#include "Application.h"
#include "BeamFactory.h"
#include "CameraManager.h"
#include "Collisions.h"
#include "IHeightFinder.h"
#include "InputEngine.h"
#include "MovableText.h"
#include "Network.h"
#include "PlayerColours.h"
#include "SurveyMapEntity.h"
#include "SurveyMapManager.h"
#include "TerrainManager.h"
#include "Utils.h"
#include "Water.h"

#include <stdint.h>

using namespace Ogre;
using namespace RoR;

#define LOGSTREAM Ogre::LogManager::getSingleton().stream()

// Named character animations.
static const char* ANIM_NAME_RUN       = "Run";
static const char* ANIM_NAME_WALK      = "Walk";
static const char* ANIM_NAME_TURN      = "Turn";
static const char* ANIM_NAME_SIDESTEP  = "Side_step";
static const char* ANIM_NAME_IDLE_SWAY = "Idle_sway";
static const char* ANIM_NAME_SWIP_LOOP = "Swim_loop";
static const char* ANIM_NAME_SWIM_SPOT = "Spot_swim";
static const char* ANIM_NAME_DRIVING   = "Driving";

static size_t s_num_local = 0; // Unique ID source
static size_t s_num_remote = 0; // Unique ID source

LocalCharacter::LocalCharacter():
    m_net_source_id(-1), // Unassigned
    m_net_stream_id(0),
    m_coupled_truck(nullptr),
    m_position(Ogre::Vector3::ZERO),
    m_rotation(0.f),
    m_speed_h(2.f),
    m_speed_v(0.f),
    m_is_visible(false),
    m_has_physics(true),
    m_can_jump(true)
{
    m_obj_name = "character_local_" + TOSTRING(s_num_local++);

    m_gfx.Load(gEnv->sceneManager, m_obj_name.c_str());
    if ((App::GetActiveMpState() == App::MP_STATE_CONNECTED) && !BSETTING("HideOwnNetLabel", false))
    {
        m_gfx.AddNetLabel(m_obj_name.c_str());
    }
}

LocalCharacter::~LocalCharacter()
{
    m_gfx.Unload(gEnv->sceneManager);
}

RemoteCharacter::~RemoteCharacter()
{
    m_gfx.Unload(gEnv->sceneManager);
}

RemoteCharacter::RemoteCharacter(int source_id, int stream_id):
    m_net_source_id(source_id),
    m_net_stream_id(stream_id),
    m_coupled_truck(nullptr)
{
    m_obj_name = "character_remote_" + TOSTRING(s_num_remote++);

    m_gfx.Load(gEnv->sceneManager, m_obj_name.c_str());
    m_gfx.AddNetLabel(m_obj_name.c_str());
    m_gfx.PushVisible(true);
}

CharacterGfx::CharacterGfx():
    m_entity     (nullptr),
    m_scene_node (nullptr),
    m_anim_states(nullptr),
    m_net_label  (nullptr)
{}

void CharacterGfx::Load(Ogre::SceneManager* scene_mgr, const char* obj_name)
{
    assert(m_entity == nullptr);
    assert(m_scene_node == nullptr);
    assert(m_anim_states == nullptr);
    
    char name_buf[50];
    // Entity
    snprintf(name_buf, 50, "%s_mesh", obj_name);
    m_entity = scene_mgr->createEntity(name_buf, "character.mesh");
    // Entity AABB (TODO: necessary?)
    AxisAlignedBox aabb;
    aabb.setInfinite();
    m_entity->getMesh()->_setBounds(aabb);
    // Material
    snprintf(name_buf, 50, "%s_material", obj_name);
    m_material = Ogre::MaterialManager::getSingleton().getByName("tracks/character")->clone(name_buf);
    m_entity->setMaterial(m_material);
    // Scene
    m_scene_node = scene_mgr->getRootSceneNode()->createChildSceneNode();
    m_scene_node->attachObject(m_entity);
    m_scene_node->setScale(0.02f, 0.02f, 0.02f); // Legacy
    // Animations
    m_anim_states = m_entity->getAllAnimationStates();
}

void CharacterGfx::AddNetLabel(const char* obj_name)
{
    assert(m_scene_node != nullptr);
    assert(m_net_label == nullptr);
    
    char name_buf[50];
    snprintf(name_buf, 50, "%s_netlabel", obj_name);
    m_net_label = new MovableText(name_buf, "");  
    m_net_label->setFontName("CyberbitEnglish");
    m_net_label->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
    m_net_label->setAdditionalHeight(2);
    m_net_label->showOnTop(false);
    m_net_label->setCharacterHeight(8);
    m_net_label->setColor(ColourValue::Black);

    m_scene_node->attachObject(m_net_label);
}

void CharacterGfx::Unload(Ogre::SceneManager* scene_mgr)
{
    assert(m_scene_node != nullptr);
    assert(m_entity != nullptr);
    
    m_scene_node->detachAllObjects();
    scene_mgr->destroySceneNode(m_scene_node);
    m_scene_node = nullptr;
    scene_mgr->destroyEntity(m_entity);
    m_entity = nullptr;
    m_anim_states = nullptr; // Invalidated by destroyEntity()
    MaterialManager::getSingleton().remove(m_material->getName());
    m_material.setNull();

    if (m_net_label != nullptr)
    {
        delete m_net_label;
        m_net_label = nullptr;
    }
}

void CharacterGfx::UpdatePlayerColor(int colornum)
{
    PlayerColours::getSingleton().updateMaterial(colornum, m_material->getName(), 2);
}

void LocalCharacter::UpdateNetLabel(Ogre::Vector3 camera_pos)
{
#ifdef USE_SOCKETW
    RoRnet::UserInfo info = RoR::Networking::GetLocalUserData();
    m_gfx.UpdatePlayerColor(info.colournum);
    if (strlen(info.username) > 0)
    {
        m_gfx.UpdateNetLabel(info.username, camera_pos, m_position);
    }
#endif    
}

void RemoteCharacter::UpdateNetLabel(Ogre::Vector3 camera_pos)
{
#ifdef USE_SOCKETW
    RoRnet::UserInfo info;
    if (!RoR::Networking::GetUserInfo(m_net_source_id, info))
        return;
    
    m_gfx.UpdatePlayerColor(info.colournum);    
    if (strlen(info.username) > 0)
    {
        m_gfx.UpdateNetLabel(info.username, camera_pos, m_position);
    }
#endif 
}

void CharacterGfx::UpdateNetLabel   (const char* caption, Ogre::Vector3 cam_pos, Ogre::Vector3 character_pos)
{
    assert(m_net_label != nullptr);

    char caption_buf[100];
    std::string utf8caption = RoR::Utils::SanitizeUtf8String(caption);
    const float cam_dist = (character_pos - cam_pos).length();
    if (cam_dist > 1000.f) // 1km
    {
        snprintf(caption_buf, 100, "%s (%.1f km)", utf8caption.c_str(), ceil(cam_dist / 100.f) / 10.0f);
        m_net_label->setCaption(utf8caption);
    }
    else if (cam_dist > 20.f)
    {
        snprintf(caption_buf, 100, "%s (%.2f m)", utf8caption.c_str(), cam_dist);
        m_net_label->setCaption(utf8caption);
    }
    else
    {
        m_net_label->setCaption(utf8caption);
    }
}

void LocalCharacter::SetPosition(Ogre::Vector3 pos)
{
    m_position = pos;
    m_prev_positions.clear();
    // TODO: update survey map
}

void CharacterGfx::PushRotation(Ogre::Radian rot_horizontal)
{
    m_scene_node->resetOrientation();
    m_scene_node->yaw(-rot_horizontal);
}

void CharacterGfx::PushPosition(Ogre::Vector3 pos)
{
    m_scene_node->setPosition(pos);
}

void  CharacterGfx::PushRotation(Ogre::Quaternion rot)
{
    m_scene_node->setOrientation(rot);
}

void CharacterGfx::PushVisible(bool vis)
{
    m_scene_node->setVisible(vis);
    //TODO: Hide on survey map!
}

void CharacterGfx::SetActiveAnim      (const char* anim_name)
{
    // Disable all
    auto itor = m_anim_states->getAnimationStateIterator();
    while (itor.hasMoreElements())
    {
        AnimationState* state = itor.getNext();
        state->setEnabled(false);
        state->setWeight(0.f);
    }
    // Enable desired
    AnimationState* state = m_anim_states->getAnimationState(anim_name);
    state->setEnabled(true);
    state->setWeight(1.f);
    m_active_anim_name = anim_name;
}

float CharacterGfx::AdvanceAnimation  (const char* anim_name, float time)
{
    assert(m_anim_states != nullptr);

    if (anim_name != m_active_anim_name)
        this->SetActiveAnim(anim_name);

    auto* state = m_anim_states->getAnimationState(anim_name);
    state->addTime(time);
    return state->getTimePosition(); // Temporary solution... the logic should not rely on read-back values
}

void CharacterGfx::PushSteerAnim  (float steer_angle)
{
    assert(m_anim_states != nullptr);

    if (m_active_anim_name != ANIM_NAME_DRIVING)
        this->SetActiveAnim(ANIM_NAME_DRIVING);

    auto* steer_anim = m_anim_states->getAnimationState(ANIM_NAME_DRIVING);
    const float anim_length = steer_anim->getLength();
    float anim_time_pos = ((steer_angle + 1.0f) * 0.5f) * anim_length;
    // prevent animation flickering on the borders:
    if (anim_time_pos < 0.01f)
    {
        anim_time_pos = 0.01f;
    }
    if (anim_time_pos > anim_length - 0.01f)
    {
        anim_time_pos = anim_length - 0.01f;
    }
    steer_anim->setTimePosition(anim_time_pos);
}

// Temporary solution... we should really update name/time locally and push later...
#define ADVANCE_ANIM_NOW(_NAME_, _TIME_) \
{ \
    m_active_anim_name = _NAME_; \
    m_active_anim_time = m_gfx.AdvanceAnimation(_NAME_, _TIME_); \
}

void LocalCharacter::Update(float dt_sec)
{
    // Clone of legacy logic
    if (!m_has_physics && m_is_coupled_visible)
    {
        Quaternion rot;
        m_coupled_truck->calculateDriverPos(m_position, rot);
        m_gfx.PushRotation(rot);
        m_position = m_position + (rot * Vector3(0.f, -0.6f, 0.f));
        m_gfx.PushPosition(m_position); // Hack to position the character right perfect on the default seat
        const float steer_angle = m_coupled_truck->hydrodirwheeldisplay * -1.0f; // Use this instead of `getSteeringAngle()` because it's smoothed
        m_gfx.PushSteerAnim(steer_angle); 
        m_active_anim_name = ANIM_NAME_DRIVING;
        m_active_anim_time = 0.f;
    }
    else if (m_has_physics)
    {  
        // disable character movement when using the free camera mode or when the menu is opened
        // TODO: check for menu being opened
        if (gEnv->cameraManager && gEnv->cameraManager->gameControlsLocked())
            return;

        // Gravity
        Ogre::Vector3 position = m_position;
        position.y += m_speed_v * dt_sec;
        m_speed_v += dt_sec * -9.8f; // TODO: respect terrain gravity settings!!

        // Auto-compensate minor height differences
        Vector3 depth_query = position + Vector3::UNIT_Y;
        while (depth_query.y > position.y)
        {
            if (gEnv->collisions->collisionCorrect(&depth_query, false))
                break;
            depth_query.y -= 0.01f; // magic!
        }
        const float depth = depth_query.y - position.y;
        if (depth > 0.f)
        {
            m_speed_v = std::max(0.f, m_speed_v);
            m_can_jump = true;
            // TODO: Legacy code was WTF - an `if (depth < 0.3f)` condition was here. ~ only_a_ptr, 01/2017
        }

        // Trigger script events and handle mesh (ground) collision
        {
            Vector3 query = position;
            gEnv->collisions->collisionCorrect(&query);
            if (std::abs(position.y - query.y) > 0.1f && gEnv->cameraManager)
            {
                gEnv->cameraManager->NotifyContextChange();
            }
            position.y = query.y;
        }

        // Obstacle detection
        if (m_prev_positions.size() > 0)
        {
            Vector3 lastPosition = m_prev_positions.front();
            Vector3 diff = m_position - lastPosition;
            Vector3 h_diff = Vector3(diff.x, 0.0f, diff.z);
            if (depth <= 0.0f || h_diff.squaredLength() > 0.0f)
            {
                const int numstep = 100;
                Vector3 base = lastPosition + Vector3::UNIT_Y * 0.5f;
                for (int i = 1; i < numstep; i++)
                {
                    Vector3 query = base + diff * ((float)i / numstep);
                    if (gEnv->collisions->collisionCorrect(&query, false))
                    {
                        position = lastPosition + diff * ((float)(i - 1) / numstep);;
                        break;
                    }
                }
            }
        }

        m_prev_positions.push_front(position);

        if (m_prev_positions.size() > 10)
        {
            m_prev_positions.pop_back();
        }

        // ground contact
        float pheight = gEnv->terrainManager->getHeightFinder()->getHeightAt(position.x, position.z);

        if (position.y < pheight)
        {
            position.y = pheight;
            m_speed_v = 0.0f;
            m_can_jump = true;
        }

        // water stuff
        bool isswimming = false;
        float wheight = -99999;

        if (gEnv->terrainManager->getWater())
        {
            wheight = gEnv->terrainManager->getWater()->getHeightWaves(position);
            if (position.y < wheight - 1.8f)
            {
                position.y = wheight - 1.8f;
                m_speed_v = 0.0f;
            }
        }

        // 0.1 due to 'jumping' from waves -> not nice looking
        if (gEnv->terrainManager->getWater() && (wheight - pheight > 1.8f) && (position.y + 0.1f <= wheight))
        {
            isswimming = true;
        }

        float tmpJoy = 0.0f;
        if (m_can_jump)
        {
            if (RoR::App::GetInputEngine()->getEventBoolValue(EV_CHARACTER_JUMP))
            {
                m_speed_v = 2.0f;
                m_can_jump = false;
            }
        }

        bool idleanim = true;

        tmpJoy = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_RIGHT);
        if (tmpJoy > 0.0f)
        {
            float scale = RoR::App::GetInputEngine()->isKeyDown(OIS::KC_LMENU) ? 0.1f : 1.0f;
            this->SetRotation(m_rotation + dt_sec * 2.0f * scale * Radian(tmpJoy));
            if (!isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_TURN, -dt_sec);
                idleanim = false;
            }
        }

        tmpJoy = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_LEFT);
        if (tmpJoy > 0.0f)
        {
            float scale = RoR::App::GetInputEngine()->isKeyDown(OIS::KC_LMENU) ? 0.1f : 1.0f;
            this->SetRotation(m_rotation - dt_sec * scale * 2.0f * Radian(tmpJoy));
            if (!isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_TURN, dt_sec);
                idleanim = false;
            }
        }

        float tmpRun = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_RUN);
        float accel = 1.0f;

        tmpJoy = accel = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_SIDESTEP_LEFT);
        if (tmpJoy > 0.0f)
        {
            if (tmpRun > 0.0f)
                accel = 3.0f * tmpRun;
            // animation missing for that
            position += dt_sec * m_speed_h * 0.5f * accel * Vector3(cos(m_rotation.valueRadians() - Math::HALF_PI), 0.0f, sin(m_rotation.valueRadians() - Math::HALF_PI));
            if (!isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_SIDESTEP, -dt_sec);
                idleanim = false;
            }
        }

        tmpJoy = accel = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_SIDESTEP_RIGHT);
        if (tmpJoy > 0.0f)
        {
            if (tmpRun > 0.0f)
                accel = 3.0f * tmpRun;
            // animation missing for that
            position += dt_sec * m_speed_h * 0.5f * accel * Vector3(cos(m_rotation.valueRadians() + Math::HALF_PI), 0.0f, sin(m_rotation.valueRadians() + Math::HALF_PI));
            if (!isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_SIDESTEP, dt_sec);
                idleanim = false;
            }
        }

        tmpJoy = accel = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_FORWARD) + RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_ROT_UP);
        float tmpBack = RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_BACKWARDS) + RoR::App::GetInputEngine()->getEventValue(EV_CHARACTER_ROT_DOWN);

        tmpJoy = std::min(tmpJoy, 1.0f);
        tmpBack = std::min(tmpBack, 1.0f);

        if (tmpJoy > 0.0f || tmpRun > 0.0f)
        {
            if (tmpRun > 0.0f)
                accel = 3.0f * tmpRun;

            float time = dt_sec * tmpJoy * m_speed_h;

            if (isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_SWIP_LOOP, time);
                idleanim = false;
            }
            else
            {
                if (tmpRun > 0.0f)
                {
                    ADVANCE_ANIM_NOW(ANIM_NAME_RUN, time);
                    idleanim = false;
                }
                else
                {
                    ADVANCE_ANIM_NOW(ANIM_NAME_WALK, time);
                    idleanim = false;
                }
            }
            // 0.005f fixes character getting stuck on meshes
            position += dt_sec * m_speed_h * 1.5f * accel * Vector3(cos(m_rotation.valueRadians()), 0.01f, sin(m_rotation.valueRadians()));
        }
        else if (tmpBack > 0.0f)
        {
            float time = -dt_sec * m_speed_h;
            if (isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_SWIM_SPOT, time);
                idleanim = false;
            }
            else
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_WALK, time);
                idleanim = false;
            }
            // 0.005f fixes character getting stuck on meshes
            position -= dt_sec * m_speed_h * tmpBack * Vector3(cos(m_rotation.valueRadians()), 0.01f, sin(m_rotation.valueRadians()));
        }

        if (idleanim)
        {
            if (isswimming)
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_SWIM_SPOT, dt_sec * 2.0f);
            }
            else
            {
                ADVANCE_ANIM_NOW(ANIM_NAME_IDLE_SWAY, dt_sec * 1.0f);
            }
        }

        m_position = position;
        // TODO: update survey map icon

        // Push updates to gfx (TODO: defer this)
        m_gfx.PushVisible(m_is_visible);
        m_gfx.PushPosition(m_position);
        m_gfx.PushRotation(m_rotation);
    }
}

void LocalCharacter::UnwindMovement(float distance)
{
    if (m_prev_positions.size() == 0)
        return;

    Ogre::Vector3 old_pos;

    for (Vector3 pos : m_prev_positions)
    {
        old_pos = pos;
        if (old_pos.distance(m_position) > distance)
            break;
    }

    m_position = old_pos;
}

void LocalCharacter::NetSendStreamSetup()
{
#ifdef USE_SOCKETW
    RoRnet::StreamRegister reg;
    memset(&reg, 0, sizeof(reg));
    reg.status = 1;
    strcpy(reg.name, "default");
    reg.type = 1;
    reg.data[0] = 2;

    RoR::Networking::AddLocalStream(&reg, sizeof(RoRnet::StreamRegister));

    m_net_source_id = reg.origin_sourceid;
    m_net_stream_id = reg.origin_streamid;
#endif // USE_SOCKETW    
}

#ifdef USE_SOCKETW
void CharacterFactory::HandleStreamData(std::vector<RoR::Networking::recv_packet_t>& packets)
{
    for (auto packet : packets)
    {
        const int command = packet.header.command;

        if (command == RoRnet::MSG2_STREAM_REGISTER)
        {
            RoRnet::StreamRegister* reg = (RoRnet::StreamRegister *)packet.buffer;
            if (reg->type == 1)
            {
                this->CreateRemoteCharacter(packet.header.source, packet.header.streamid);
            }
        }
        else if (command == RoRnet::MSG2_USER_LEAVE)
        {
            this->DeleteRemoteCharacter(packet.header.source);
        }
        else if (command == RoRnet::MSG2_STREAM_DATA)
        {
            for (auto& c : m_remote_objs)
            {
                if (packet.header.source = c->GetSourceId() && packet.header.streamid == c->GetStreamId())
                {
                    c->HandleStreamData(packet.buffer);
                    break;
                }
            }
        }
    }
}

// Helper function
void RemoteCharacter::ReportError(const char* detail)
{
    Ogre::UTFString username;
    RoRnet::UserInfo info;
    if (!RoR::Networking::GetUserInfo(m_net_source_id, info))
        username = "~~ERROR getting username~~";
    else
        username = info.username;

    char msg_buf[300];
    snprintf(msg_buf, 300, 
        "[RoR|Networking] ERROR on remote character (User: '%s', SourceID: %d, StreamID: %d): ",
        username.asUTF8_c_str(), m_net_source_id, m_net_stream_id);

    LOGSTREAM << msg_buf << detail;
}

#endif // USE_SOCKETW
void RemoteCharacter::HandleStreamData(char* buffer)
{
    const auto* msg = reinterpret_cast<Networking::CharacterMsgGeneric*>(buffer);
    if (msg->command == Networking::CHARACTER_CMD_ATTACH)
    {
        const auto* attach_msg = reinterpret_cast<Networking::CharacterMsgAttach*>(buffer);
        Beam* beam = BeamFactory::getSingleton().getBeam(attach_msg->source_id, attach_msg->stream_id);
        if (beam != nullptr)
            this->AttachToTruck(beam);
        else
        {
            char err_buf[200];
            snprintf(err_buf, 200, "Received command `ATTACH` with target{SourceID: %d, StreamID: %d}, "
                "but corresponding vehicle doesn't exist. Ignoring command.",
                attach_msg->source_id, attach_msg->stream_id);
            this->ReportError(err_buf);
        }
    }
    else if (msg->command == Networking::CHARACTER_CMD_DETACH)
    {
        if (m_coupled_truck != nullptr)
        {
            this->DetachFromTruck();
        }
        else
        {
            this->ReportError("Received command `DETACH`, but not currently attached to a vehicle. Ignoring command.");
        }
    }
    else if (msg->command == Networking::CHARACTER_CMD_POSITION)
    {
        const auto* msg = reinterpret_cast<Networking::CharacterMsgPos*>(buffer);
        m_position = Ogre::Vector3(msg->pos_x, msg->pos_y, msg->pos_z);
        m_gfx.PushPosition(m_position);
        m_gfx.PushRotation(Ogre::Radian(msg->rot_angle));
        m_gfx.AdvanceAnimation(Utils::SanitizeUtf8String(msg->anim_name).c_str(), msg->anim_time);
    }
    else
    {
        char err_buf[100];
        snprintf(err_buf, 100, "Received invalid command: %d. Cannot process.", msg->command);
        this->ReportError(err_buf);
    }
}

void RemoteCharacter::AttachToTruck(Beam* truck)
{
    assert(truck != nullptr);
    assert(m_coupled_truck == nullptr);

    m_coupled_truck = truck;
    m_gfx.SetNetLabelVisible(false);

    if (!truck->hasDriverSeat())
        m_gfx.PushVisible(false);
    else
        m_gfx.SetCastShadows(false); // Do not cast in vehicle
}

void LocalCharacter::AttachToTruck(Beam* truck)
{
    assert(truck != nullptr);

    m_coupled_truck = truck;
    m_has_physics = false;
    m_gfx.SetNetLabelVisible(false);
    m_is_coupled_visible = truck->hasDriverSeat();

    if (!m_is_coupled_visible)
    {
        m_is_visible = false;
        m_gfx.PushVisible(false); // TODO: defer this...
    }
    else
    {
        m_gfx.SetCastShadows(false); // Do not cast in vehicle
    }

    if (App::GetActiveMpState() == App::MP_STATE_CONNECTED)
    {
        Networking::CharacterMsgAttach msg;
        msg.command = Networking::CHARACTER_CMD_ATTACH;
        msg.source_id = m_coupled_truck->m_source_id;
        msg.stream_id = m_coupled_truck->m_stream_id;
        const int msg_size = sizeof(Networking::CharacterMsgAttach);
        Networking::AddPacket(m_net_stream_id, RoRnet::MSG2_STREAM_DATA, msg_size, (char*)&msg);
    }
}

void RemoteCharacter::DetachFromTruck()
{
    assert(m_coupled_truck != nullptr);

    m_coupled_truck = nullptr;
    m_gfx.PushVisible(true);
    m_gfx.PushRotation(Ogre::Radian(0.f));
    m_gfx.SetCastShadows(true);
    m_gfx.SetNetLabelVisible(true);
}

void LocalCharacter::DetachFromTruck()
{
    assert(m_coupled_truck != nullptr);

    m_coupled_truck = nullptr;
    m_has_physics = true;
    m_is_coupled_visible = false;
    m_is_visible = true;
    m_gfx.PushVisible(true); // TODO: defer this...
    m_gfx.PushRotation(Ogre::Radian(0.f));
    m_gfx.SetCastShadows(true);
    m_gfx.SetNetLabelVisible(true);

    if (App::GetActiveMpState() == App::MP_STATE_CONNECTED)
    {
        Networking::CharacterMsgGeneric msg;
        msg.command = Networking::CHARACTER_CMD_DETACH;
        const int msg_size = sizeof(Networking::CharacterMsgGeneric);
        Networking::AddPacket(m_net_stream_id, RoRnet::MSG2_STREAM_DATA, msg_size, (char*)&msg);
    }
}

void CharacterGfx::SetNetLabelVisible (bool visible)
{
    if (m_net_label != nullptr)
        m_net_label->setVisible(visible);
}

void CharacterGfx::SetCastShadows     (bool v)
{
    assert(m_entity != nullptr);
    m_entity->setCastShadows(v);
}

void LocalCharacter::NetSendState()
{
#ifdef USE_SOCKETW
    if (m_coupled_truck)
        return;

    Networking::CharacterMsgPos msg;
    msg.command          = Networking::CHARACTER_CMD_POSITION;
    msg.pos_x            = m_position.x;
    msg.pos_y            = m_position.y;
    msg.pos_z            = m_position.z;
    msg.rot_angle        = m_rotation.valueRadians();
    strncpy(msg.anim_name, m_active_anim_name, CHARACTER_ANIM_NAME_LEN);
    msg.anim_time        = m_active_anim_time;
#endif // USE_SOCKETW
}

CharacterFactory::CharacterFactory():
    m_local_obj(nullptr)
{
}

LocalCharacter* CharacterFactory::CreateLocalCharacter(int playerColour)
{
    m_local_obj = new LocalCharacter();
    return m_local_obj;
}

RemoteCharacter* CharacterFactory::CreateRemoteCharacter(int source_id, int stream_id)
{
#ifdef USE_SOCKETW
    RoRnet::UserInfo info;
    RoR::Networking::GetUserInfo(source_id, info);
    int colour = info.colournum;

    char msg_buf[400];
    snprintf(msg_buf, 400, "[RoR|Networking] Creating character for user '%s' (SourceID: %d, StreamID: %d, ColorNum: %d)",
        Utils::SanitizeUtf8String(info.username).c_str(), source_id, stream_id, info.colournum);
    LOG(msg_buf);

    auto* obj = new RemoteCharacter(source_id, stream_id);
    m_remote_objs.push_back(obj);
    return obj;
#endif // USE_SOCKETW
}

void CharacterFactory::DeleteAllCharacters()
{
    delete m_local_obj; // Destructor does all the cleanup
    m_local_obj = nullptr;

    while (! m_remote_objs.empty())
    {
        delete m_remote_objs.back();
        m_remote_objs.pop_back();
    }
}

void CharacterFactory::Update(float dt_sec, Ogre::Vector3 camera_pos)
{
    m_local_obj->Update(dt_sec);
    if (App::GetActiveMpState() == App::MP_STATE_CONNECTED)
    {
        m_local_obj->UpdateNetLabel(camera_pos);
        m_local_obj->NetSendState();
    }

    for (RemoteCharacter* rem : m_remote_objs)
    {
        rem->UpdateNetLabel(camera_pos);
    }
}

void CharacterFactory::DeleteRemoteCharacter(int source_id)
{
    auto itor = m_remote_objs.begin();
    auto endi = m_remote_objs.end();
    for (; itor != endi; ++itor)
    {
        RemoteCharacter* r = *itor;
        if (r->GetSourceId() == source_id)
        {
            delete r;
            m_remote_objs.erase(itor);
            return;
        }
    }
}


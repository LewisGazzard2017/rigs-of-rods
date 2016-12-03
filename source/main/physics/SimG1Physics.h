
#pragma once

// Project NextSim
// ===============
// This is a rework of RoR's traditional physics engine
// named "Beam" and retro-labeled as "G1"

#include <OgreVector3.h>

struct G1Node
{
    Ogre::Vector3 abs_pos;
    Ogre::Vector3 rel_pos;
};

class G1Actor
{
public:
    void TranslateOrigin(Ogre::Vector3 offset);
    void BeginUpdate();

private:
    Ogre::Vector3       m_origin;
    std::vector<G1Node> m_nodes;
    Ogre::Vector3       m_avg_pos;
    Ogre::Vector3       m_prev_avg_pos;
};

class G1LogicContext
{
public:
    void BeginUpdate(size_t dt_milis);
    void PreUpdatePhysics();

private:
    size_t              m_num_frames;
    float               m_sim_speed;      ///< slow motion < 1.0 < fast motion
    float               m_dt_remainder;   ///< Keeps track of the rounding error in the time step calculation
    size_t              m_update_num_steps;
    std::list<G1Actor>  m_actors;
};


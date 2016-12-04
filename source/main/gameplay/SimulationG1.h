
#pragma once

// Project NextSim
// ===============
// This is a rework of RoR's traditional physics engine
// named "Beam" and retro-labeled as "G1"

#include <OgreVector3.h>

// Forward
struct G1Shock;

struct G1Node
{
    Ogre::Vector3 abs_pos;
    Ogre::Vector3 rel_pos;
    Ogre::Vector3 velocity;
};

// Equals beam_t
struct G1Beam
{
    G1Node*   p1;
    G1Node*   p2;
    float     base_len;
    float     spring;
    float     damp;
    float     short_bound;
    float     long_bound;

    // Type bits
    bool      is_shock1:1;      // Equals (beam_t::bounded == SHOCK1)
    bool      is_shock2:1;      // Equals (beam_t::bounded == SHOCK2)
    bool      is_support:1;     // Equals (beam_t::bounded == SUPPORTBEAM)
    bool      is_rope:1;        // Equals (beam_t::bounded == ROPE)
    bool      is_hydro:1;       // Equals (beam_t::type == BEAM_HYDRO)
    bool      is_invis_hydro:1; // Equals (beam_t::type == BEAM_INVISIBLE_HYDRO)
    bool      is_inter_actor:1; // Equals (beam_t::p2truck == true)
    // State bits
    bool      is_broken:1;
    bool      is_disabled;

    // < -- 64 Bytes -->

    G1Shock*  shock;
};

struct G1Shock
{
    float sbd_spring; ///< SBD = `set_beam_defaults` directive from truckfile | used by both SHOCK1 and SHOCK2
    float sbd_damp;   ///< SBD = `set_beam_defaults` directive from truckfile | used by both SHOCK1 and SHOCK2
};

class G1Actor
{
public:
    // Read/write: nodes
    void TranslateOrigin(Ogre::Vector3 offset);
    // Read/write: nodes
    void BeginUpdate();
    // Read: nodes, beams; Write: beams
    void UpdateBeams();

private:
    static void UpdateBeamShock1(G1Beam& beam, float cur_len_diff, float& spring, float& damp);

    Ogre::Vector3         m_origin;
    std::vector<G1Node>   m_nodes;
    std::vector<G1Beam>   m_beams;
    Ogre::Vector3         m_avg_pos;
    Ogre::Vector3         m_prev_avg_pos;
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


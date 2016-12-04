
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
    short         pos;
    Ogre::Vector3 abs_pos;
    Ogre::Vector3 rel_pos;
    Ogre::Vector3 velocity;
    Ogre::Vector3 forces;

    // Type bits
    bool          is_contacter:1;
};

// Equals beam_t
struct G1Beam
{
    G1Node*   p1;
    G1Node*   p2;
    float     spring;
    float     damp;
    float     length;
    float     stress;
    float     deform_thr_abs;      // Formerly 'minmaxposnegstress'
    float     deform_thr_compress; // Formerly 'maxposstress' 
    float     deform_thr_expand;   // Formerly 'maxnegstress'
    float     plastic_coef;
    float     strength;

    // Type bits
    bool      is_shock1:1;      // Equals (beam_t::bounded == SHOCK1)
    bool      is_shock2:1;      // Equals (beam_t::bounded == SHOCK2)
    bool      is_support:1;     // Equals (beam_t::bounded == SUPPORTBEAM)
    bool      is_rope:1;        // Equals (beam_t::bounded == ROPE)
    bool      is_normal:1;      // Equals (beam_t::type == BEAM_NORMAL)
    bool      is_invis:1;       // Equals (beam_t::type == BEAM_INVISIBLE)
    bool      is_hydro:1;       // Equals (beam_t::type == BEAM_HYDRO)
    bool      is_invis_hydro:1; // Equals (beam_t::type == BEAM_INVISIBLE_HYDRO)
    bool      is_inter_actor:1; // Equals (beam_t::p2truck == true)
    // State bits
    bool      is_broken:1;
    bool      is_disabled:1;
    // Total bits: 11 => 2 byte bitfield

    // < -- 64 Bytes -->

    float     short_bound;
    float     long_bound;
    G1Shock*  shock;
};

struct G1Shock
{
    float sbd_spring;     ///< SBD = `set_beam_defaults` directive from truckfile | used by both SHOCK1 and SHOCK2
    float sbd_damp;       ///< SBD = `set_beam_defaults` directive from truckfile | used by both SHOCK1 and SHOCK2
    float last_len_diff;  ///< Last step's length diff from base
    float spring_in;
    float spring_in_prog;
    float spring_out;
    float spring_out_prog;
    float damp_in;
    float damp_in_prog;
    float damp_out;
    float damp_out_prog;
    float trigger_switch_state;     ///< needed to avoid doubleswitch, bool and timer in one
    // Type bits
    bool is_soft_bump:1;
    bool is_normal:1;
    bool is_trigger:1;
    bool is_trig_blocker:1;
    // State bits
    bool is_trigger_enabled:1;
};

/// Node/Beam connectivity graph.
// Replaces following fields of old actor:
//    std::vector< std::vector< int > > nodetonodeconnections;
//    std::vector< std::vector< int > > nodebeamconnections;
class G1SoftbodyGraph
{
public:
    struct NodeConn ///< Per-node info about connected elements
    {
        size_t nodes_index;
        size_t nodes_count;
        size_t beams_index;
        size_t beams_count;
    };

    void    Calculate(std::vector<G1Node>& nodes, std::vector<G1Beam>& beams);
    G1Node* GetNode(size_t index);
    G1Beam* GetBeam(size_t index);
    size_t  GetNumActiveNeighborBeams(int node_id);  ///< Returns the number of active (non bounded) beams connected to a node

private:
    std::vector<NodeConn> m_node_info; ///< Per-node info
    std::vector<G1Node*>  m_nodes; ///< Per-node info of connected nodes; Logical array of arrays.
    std::vector<G1Beam*>  m_beams; ///< Per-node info of connected beams; Logical array of arrays.
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
    // Reads { Beam, Shock }
    static void UpdateBeamShock1(G1Beam& beam, float cur_len_diff, float& spring, float& damp);
    // Reads { Beam, Shock }
    static void UpdateBeamShock2(G1Beam& beam, float cur_len_diff, float& spring, float& damp);
    // Reads { Beam }; Writes { Actor.increase_col_accuracy, Beam }
    void UpdateBeamDeform(G1Beam& beam, float& slen, float len, const float cur_len_diff, const float spring);
    // Reads { Beam, SoftbodyGraph }; Writes { Beam }
    void UpdateBeamBreaking(G1Beam& beam, float& slen);

    Ogre::Vector3         m_origin;
    std::vector<G1Node>   m_nodes;
    std::vector<G1Beam>   m_beams;
    Ogre::Vector3         m_avg_pos;
    Ogre::Vector3         m_prev_avg_pos;
    G1SoftbodyGraph       m_softbody_graph;

    struct
    {
        // Formerly `bool Beam::increased_accuracy` - Starts 'false', adjusted in `calcBeams()`, affects `calcNodes()`
        bool increase_coll_accuracy;
    }                     m_step_context; ///< Data of current physics step only
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


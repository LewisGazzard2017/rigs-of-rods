
#pragma once

namespace RoR {

const int TOBJ_STR_LEN 300;
const int TOBJ_LINE_BUF_LEN 2000;

// -----------------------------------------------------------------------------
struct TObjTree
{                     
    TObjTree():
        yaw_from(0.f),            yaw_to(0.f),
        scale_from(0.f),          scale_to(0.f),
        min_distance(90),         max_distance(700),
        high_density(1.f),
        grid_spacing(0.f),
        
        tree_mesh     [0]('\0'),
        color_map     [0]('\0'),
        density_map   [0]('\0'),
        collision_mesh[0]('\0'),
    {}
    
    float    yaw_from,        yaw_to;
    float    scale_from,      scale_to;
    float    min_distance,    max_distance;        
    float    high_density;
    float    grid_spacing;
    
    char     tree_mesh[TOBJ_STR_LEN];
    char     color_map[TOBJ_STR_LEN];
    char     density_map[TOBJ_STR_LEN];
    char     collision_mesh[TOBJ_STR_LEN];   
};

// -----------------------------------------------------------------------------
/// Unified 'grass' and 'grass2'
struct TObjGrass
{
    TObjGrass():
        range(80),
        technique(GRASSTECH_CROSSQUADS),
        grow_techniq(0),
        sway_speed(0.5f),
        sway_length(0.05f),
        sway_distrib(10.f),
        
        min_x(0.2f),   min_y(0.2f),   min_h(-9999.f),
        max_x(1.0f),   max_y(0.6f),   max_h(+9999.f),
        
        grass_mat_name       [0]('\0'),
        color_map_filename   [0]('\0'),
        density_map_filename [0]('\0')
    {}
    
    int      range;
    int      technique;
    int      grow_techniq;
    float    sway_speed;
    float    sway_length;
    float    sway_distrib;
    float    density;
    
    float    min_x,   min_y,   min_h;
    float    max_x,   max_y,   max_h;
    
    char     grass_mat_name[TOBJ_STR_LEN];
    char     color_map_filename[TOBJ_STR_LEN];
    char     density_map_filename[TOBJ_STR_LEN];
};

// -----------------------------------------------------------------------------
struct TObjRig
{
    Ogre::Vector3      position;
    Ogre::Quaternion   rotation;
    char               name[TOBJ_STR_LEN];
    bool               is_machine;
    bool               free_position;
};

// -----------------------------------------------------------------------------
struct TObjEntry
{
    Ogre::Vector3      position;
    Ogre::Vector3      rotation;
    char               obj_name[TOBJ_STR_LEN];
    char               type[TOBJ_STR_LEN];
    char               name[TOBJ_STR_LEN];
};

// -----------------------------------------------------------------------------
struct TObjFile
{
    TObjFile():
        num_collision_triangles(Collisions::MAX_COLLISION_TRIS),
        grid_position(),
        grid_enabled(false)
    {}
    
    int                     num_collision_triangles;
    Ogre::Vector3           grid_position;
    bool                    grid_enabled;
    std::vector<TObjTree>   trees;
    std::vector<TObjGrass>  grass;
    std::vector<TObjRig>    rigs;
    std::vector<TObjEntry>  objects;
};

// -----------------------------------------------------------------------------
class TObjParser
{
public:
    void Prepare();
    void ProcessLine(const char* line);

private:
    void ProcessCurrentLine();

    std::shared_ptr<TObjFile>  m_def;
    int                        m_line_number;
    const char*                m_cur_line;
    bool                       m_in_procedural_road; // Old parser: 'bool proroad'
    bool                       m_road2_use_old_mode; // Old parser: 'int r2oldmode' 
};

} // namespace RoR

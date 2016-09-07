
#pragma once

struct TObjTree
{                     
    TObjTree():
        yaw_from    (0.f),
        yaw_to      (0.f),
        scale_from  (0.f),
        scale_to    (0.f),
        high_density(1.f),
        min_distance(90),
        max_distance(700),
        grid_spacing(0.f),
        
        tree_mesh          [0]('\0'),
        color_map          [0]('\0'),
        density_map        [0]('\0'),
        tree_collision_mesh[0]('\0'),
    {}
    
    float    yaw_from;
    float    yaw_to;
    float    scale_from;
    float    scale_to;
    float    high_density;
    float    min_distance;
    float    max_distance;
    float    grid_spacing;
    char     tree_mesh[300];
    char     color_map[300];
    char     density_map[300];
    char     tree_collision_mesh[300];   
};

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
};

} // namespace TObj
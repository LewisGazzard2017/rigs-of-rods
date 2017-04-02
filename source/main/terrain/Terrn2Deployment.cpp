
#include "Terrn2Deployment.h"

#include "RoRPrerequisites.h"
#include "TObjFileFormat.h"
#include "Utils.h"

#include "PagedGeometry.h"
#include "TreeLoader3D.h"

using namespace RoR;

#define LOGSTREAM Ogre::LogManager::getSingleton().stream() << "[RoR|DeployTerrn2] "
#define ERRSTREAM LOGSTREAM << "ERROR in [" << m_terrn2.name << "]:"

bool Terrn2Deployer::DeployTerrn2(std::string terrn2_filename)
{
    auto& ogre_resources = Ogre::ResourceGroupManager::getSingleton();

    // PROCESS .TERRRN2
    try
    {
        // create the Stream
        const std::string group_name = ogre_resources.findGroupContainingResource(terrn2_filename);
        Ogre::DataStreamPtr stream = ogre_resources.openResource(terrn2_filename, group_name);

        // Parse the file
        Terrn2Parser parser;
        const bool loaded_ok = parser.LoadTerrn2(m_terrn2, stream);

        // Report messages
        if (parser.GetMessages().size() > 0)
        {
            LOGSTREAM << "Terrn2 parser messages:";
            for (std::string const& msg : parser.GetMessages())
                LOGSTREAM << '\t' << msg;
        }

        // Error?
        if (!loaded_ok)
            return false;
    }
    catch (...)
    {
        this->HandleException("processing .terrn2 file");
    }

    // PROCESS .OTC (ogre terrain conf)
    try
    {
        if (! this->CheckAndLoadOTC())
        {
            return false; // Error already logged
        }
    }
    catch (...)
    {
        this->HandleException("processing .otc file(s)");
    }

    // LOAD TOBJ FILES (terrain objects)
    try
    {
        for (std::string const& tobj_filename : m_terrn2.tobj_files)
        {
            // create the Stream
            const std::string group_name = ogre_resources.findGroupContainingResource(tobj_filename);
            Ogre::DataStreamPtr stream = ogre_resources.openResource(tobj_filename, group_name);

            // Parse the file
            TObjParser parser;
            parser.Prepare();
            bool error = false;
            while (!stream->eof())
            {
                if (!parser.ProcessLine(stream->getLine().c_str()))
                {
                    LOGSTREAM << "Error reading .tobj file '" << tobj_filename << "'";
                    error = true;
                    break;
                }
            }

            // Persist the data
            if (!error)
                m_tobj_list.push_back(parser.Finalize());
        }
    }
    catch (...)
    {
        this->HandleException("processing .tobj file(s)");
    }

    // COMPOSE JSON
    m_json_root = Json::objectValue;
    this->ProcessTerrn2Json();
    this->ProcessOtcJson();
    this->ProcessTobjJson();

    // TODO: write out JSON
}

bool Terrn2Deployer::CheckAndLoadOTC()
{
    if (m_terrn2.ogre_ter_conf_filename.empty())
    {
        return true;
    }

    auto& ogre_resources = Ogre::ResourceGroupManager::getSingleton();
    const std::string& group_name = ogre_resources.findGroupContainingResource(m_terrn2.ogre_ter_conf_filename);
    Ogre::DataStreamPtr otc_stream = ogre_resources.openResource(m_terrn2.ogre_ter_conf_filename, group_name);

    OTCParser parser;
    if (! parser.LoadMasterConfig(otc_stream, m_terrn2.ogre_ter_conf_filename.c_str()))
    {
        return false; // Error already logged
    }

    for (OTCPage& page : parser.GetDefinition()->pages)
    {
        const std::string& page_group_name = ogre_resources.findGroupContainingResource(m_terrn2.ogre_ter_conf_filename);
        Ogre::DataStreamPtr page_stream = ogre_resources.openResource(m_terrn2.ogre_ter_conf_filename, group_name);

        if (! parser.LoadPageConfig(page_stream, page))
        {
            return false; // Error already logged
        }
    }

    m_otc = parser.GetDefinition();
    return true;
}

void Terrn2Deployer::ProcessOtcJson()
{
    Json::Value json_otc = Json::objectValue;

    json_otc["world_size_x"]            = m_otc->world_size_x           ;
    json_otc["world_size_y"]            = m_otc->world_size_y           ;
    json_otc["world_size_z"]            = m_otc->world_size_z           ;
    json_otc["page_size"]               = m_otc->page_size              ;
    json_otc["world_size"]              = m_otc->world_size             ;
    json_otc["pages_max_x"]             = m_otc->pages_max_x            ;
    json_otc["pages_max_z"]             = m_otc->pages_max_z            ;
    json_otc["max_pixel_error"]         = m_otc->max_pixel_error        ;
    json_otc["batch_size_min"]          = m_otc->batch_size_min         ;
    json_otc["batch_size_max"]          = m_otc->batch_size_max         ;
    json_otc["layer_blendmap_size"]     = m_otc->layer_blendmap_size    ;
    json_otc["composite_map_size"]      = m_otc->composite_map_size     ;
    json_otc["composite_map_distance"]  = m_otc->composite_map_distance ;
    json_otc["skirt_size"]              = m_otc->skirt_size             ;
    json_otc["lightmap_size"]           = m_otc->lightmap_size          ;
    json_otc["lightmap_enabled"]        = m_otc->lightmap_enabled       ;
    json_otc["norm_map_enabled"]        = m_otc->norm_map_enabled       ;
    json_otc["spec_map_enabled"]        = m_otc->spec_map_enabled       ;
    json_otc["parallax_enabled"]        = m_otc->parallax_enabled       ;
    json_otc["global_colormap_enabled"] = m_otc->global_colormap_enabled;
    json_otc["recv_dyn_shadows_depth"]  = m_otc->recv_dyn_shadows_depth ;
    json_otc["blendmap_dbg_enabled"]    = m_otc->blendmap_dbg_enabled   ;
    json_otc["disable_cache"]           = m_otc->disable_cache          ;
    json_otc["is_flat"]                 = m_otc->is_flat                ;

    json_otc["pages"] = Json::arrayValue;
    for (OTCPage& page : m_otc->pages)
    {
        Json::Value jpage = Json::objectValue;
        jpage["layerconf_filename"]   = page.layerconf_filename  ;
        jpage["heightmap_filename"]   = page.heightmap_filename  ;
        jpage["was_layerconf_parsed"] = page.was_layerconf_parsed;
        jpage["num_layers"]           = page.num_layers          ;
        jpage["pos_x"]                = page.pos_x               ;
        jpage["pos_z"]                = page.pos_z               ;
        jpage["is_heightmap_raw"]     = page.is_heightmap_raw    ;
        jpage["raw_flip_x"]           = page.raw_flip_x          ;
        jpage["raw_flip_y"]           = page.raw_flip_y          ;
        jpage["raw_size"]             = page.raw_size            ;
        jpage["raw_bpp"]              = page.raw_bpp             ;

        jpage["layers"] = Json::arrayValue;
        for (OTCLayer& layer : page.layers)
        {
            Json::Value jlayer = Json::objectValue;
            jlayer["blendmap_filename"]        = layer.blendmap_filename       ;
            jlayer["diffusespecular_filename"] = layer.diffusespecular_filename;
            jlayer["normalheight_filename"]    = layer.normalheight_filename   ;
            jlayer["blend_mode"]               = layer.blend_mode              ;
            jlayer["alpha"]                    = layer.alpha                   ;
            jlayer["world_size"]               = layer.world_size              ;

            jpage["layers"].append(jlayer);
        }

        json_otc["pages"].append(jpage);
    }

    m_json_root["ogre_terrain_conf"] = json_otc;
}

void Terrn2Deployer::ProcessTerrn2Json()
{
    Json::Value t2_amb_color;
    t2_amb_color["r"] = m_terrn2.ambient_color.r;
    t2_amb_color["g"] = m_terrn2.ambient_color.g;
    t2_amb_color["b"] = m_terrn2.ambient_color.b;
    t2_amb_color["a"] = m_terrn2.ambient_color.a;

    Json::Value t2_start_pos;
    t2_start_pos["x"] = m_terrn2.start_position.x;
    t2_start_pos["y"] = m_terrn2.start_position.y;
    t2_start_pos["z"] = m_terrn2.start_position.z;

    Json::Value json_terrn2 = Json::objectValue;
    json_terrn2["ambient_color"]        = t2_amb_color;
    json_terrn2["start_position"]       = t2_start_pos;
    json_terrn2["category_id"]          = m_terrn2.category_id        ;  // Int
    json_terrn2["guid"]                 = m_terrn2.guid               ;  // String
    json_terrn2["version"]              = m_terrn2.version            ;  // Int
    json_terrn2["gravity"]              = m_terrn2.gravity            ;  // Float
    json_terrn2["caelum_config"]        = m_terrn2.caelum_config      ;  // String
    json_terrn2["cubemap_config"]       = m_terrn2.cubemap_config     ;  // String
    json_terrn2["caelum_fog_start"]     = m_terrn2.caelum_fog_start   ;  // Int
    json_terrn2["caelum_fog_end"]       = m_terrn2.caelum_fog_end     ;  // Int
    json_terrn2["has_water"]            = m_terrn2.has_water          ;  // Bool
    json_terrn2["hydrax_conf_file"]     = m_terrn2.hydrax_conf_file   ;  // String
    json_terrn2["traction_map_file"]    = m_terrn2.traction_map_file  ;  // String
    json_terrn2["water_height"]         = m_terrn2.water_height       ;  // Float
    json_terrn2["water_bottom_height"]  = m_terrn2.water_bottom_height;  // Float

    m_json_root["general"] = json_terrn2;
}

void Terrn2Deployer::ProcessTobjJson()
{
    Json::Value json_tobj = Json::objectValue;

    for (std::shared_ptr<TObjFile> tobj : m_tobj_list)
    {
        // Grid
        if (tobj->grid_enabled)
        {
            json_tobj["grid_enabled"] = Json::Value(true);
            json_tobj["grid_position_x"] = Json::Value(tobj->grid_position.x);
            json_tobj["grid_position_y"] = Json::Value(tobj->grid_position.y);
            json_tobj["grid_position_z"] = Json::Value(tobj->grid_position.z);
        }

        // Collision triangle count
        Json::Value json_num_tris = json_tobj["num_collision_triangles"];
        if (json_num_tris.isNull() || (json_num_tris.isIntegral() && (tobj->num_collision_triangles > static_cast<long>(json_num_tris.asInt()))))
        {
            json_tobj["num_collision_triangles"] = Json::Value(tobj->num_collision_triangles);
        }
    }

    m_json_root["terrain_objects"] = json_tobj;
    m_json_root["collision_meshes"] = Json::arrayValue;

    m_json_root["terrain_objects"]["grass"] = Json::arrayValue;
    this->ProcessTobjGrassJson();

    m_json_root["terrain_objects"]["trees"] = Json::arrayValue;
    this->ProcessTobjTreesJson();
}

void Terrn2Deployer::ProcessTobjGrassJson()
{
    for (std::shared_ptr<TObjFile> tobj : m_tobj_list)
    {
        for (TObjGrass& grass : tobj->grass)
        {
            Json::Value json;
            json["range"]        = grass.range;
            json["technique"]    = grass.technique;
            json["grow_techniq"] = grass.grow_techniq;
            json["sway_speed"]   = grass.sway_speed;
            json["sway_length"]  = grass.sway_length;
            json["sway_distrib"] = grass.sway_distrib;
            json["density"]      = grass.density;

            json["min_x"] = grass.min_x;
            json["min_y"] = grass.min_y;
            json["min_h"] = grass.min_h;
            json["max_x"] = grass.max_x;
            json["max_y"] = grass.max_y;
            json["max_h"] = grass.max_h;

            json["material_name"]        = grass.material_name;
            json["color_map_filename"]   = grass.color_map_filename;
            json["density_map_filename"] = grass.density_map_filename;

            m_json_root["terrain_objects"]["grass"].append(json);
        }
    }
}

void Terrn2Deployer::ProcessTobjTreesJson()
{
    for (std::shared_ptr<TObjFile> tobj : m_tobj_list)
    {
        for (TObjTree& tree : tobj->trees)
        {
            if (tree.color_map[0] == 0) // Empty c-string?
            {
                LOGSTREAM << "TObj/Tree has no colormap, skipping...";
                continue;
            }
            if (tree.density_map[0] == 0) // Empty c-string?
            {
                LOGSTREAM << "TObj/Tree has no density-map, skipping...";
                continue;
            }
            Forests::DensityMap* density_map = Forests::DensityMap::load(tree.density_map, Forests::CHANNEL_COLOR);
            if (density_map == nullptr)
            {
                LOGSTREAM << "TObj/Tree: couldn't load density map [" << tree.density_map << "], skipping...";
                continue;
            }

            // Generate trees
            float max_x = (float) m_otc->world_size_x;
            float max_z = (float) m_otc->world_size_z;
            Ogre::TRect<float> bounds = Forests::TBounds(0, 0, max_x, max_z);
            if (tree.grid_spacing > 0)
            {
                // Grid style
                for (float x = 0; x < max_x; x += tree.grid_spacing)
                {
                    for (float z = 0; z < max_z; z += tree.grid_spacing)
                    {
                        float density = density_map->_getDensityAt_Unfiltered(x, z, bounds);
                        if (density < 0.8f)
                        {
                            continue;
                        }
                        float pos_x = x + tree.grid_spacing * 0.5f;
                        float pos_z = z + tree.grid_spacing * 0.5f;
                        float yaw = Ogre::Math::RangeRandom(tree.yaw_from, tree.yaw_to);
                        float scale = Ogre::Math::RangeRandom(tree.scale_from, tree.scale_to);
                        Json::Value jtree = Json::objectValue;
                        jtree["pos_x"] = pos_x;
                        jtree["pos_z"] = pos_z;
                        jtree["yaw"]   = yaw;
                        jtree["scale"] = scale;
                        m_json_root["terrain_objects"]["trees"].append(jtree);
                        if (tree.collision_mesh[0] != 0) // Non-empty cstring?
                        {
                            Ogre::Quaternion rot(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);
                            Ogre::Vector3 scale3d(scale * 0.1f, scale * 0.1f, scale * 0.1f);
                            this->AddCollMeshJson(tree.collision_mesh, pos_x, pos_z, rot, scale3d);
                        }
                    }
                }
            }
            else
            {
                // Normal style, random
                float hi_density = tree.high_density;
                float grid_size = 10;
                if (tree.grid_spacing < 0 && tree.grid_spacing != 0) // Verbatim port of old logic
                {
                    grid_size = -tree.grid_spacing;
                }
                for (float x = 0; x < max_x; x += tree.grid_spacing)
                {
                    for (float z = 0; z < max_z; z += tree.grid_spacing)
                    {
                        if (tree.high_density < 0)
                        {
                            hi_density = Ogre::Math::RangeRandom(0, -tree.high_density);
                        }
                        float density = density_map->_getDensityAt_Unfiltered(x, z, bounds);
                        // Pre-generate enough trees for the highest settings
                        int num_trees = static_cast<int>(hi_density * density * 1.0f) + 1;
                        for (int i = 0; i < num_trees; ++i)
                        {
                            float pos_x = Ogre::Math::RangeRandom(x, x + grid_size);
                            float pos_z = Ogre::Math::RangeRandom(z, z + grid_size);
                            float yaw = Ogre::Math::RangeRandom(tree.yaw_from, tree.yaw_to);
                            float scale = Ogre::Math::RangeRandom(tree.scale_from, tree.scale_to);
                            Json::Value jtree = Json::objectValue;
                            jtree["pos_x"] = pos_x;
                            jtree["pos_z"] = pos_z;
                            jtree["yaw"]   = yaw;
                            jtree["scale"] = scale;
                            m_json_root["terrain_objects"]["trees"].append(jtree);
                            if (tree.collision_mesh[0] != 0)
                            {
                                Ogre::Quaternion rot(Ogre::Degree(yaw), Ogre::Vector3::UNIT_Y);
                                Ogre::Vector3 scale3d(scale * 0.1f, scale * 0.1f, scale * 0.1f);
                                this->AddCollMeshJson(tree.collision_mesh, pos_x, pos_z, rot, scale3d);
                            }
                        }
                    }
                }
            }

        }
    }
}

void Terrn2Deployer::AddCollMeshJson(const char* name, float pos_x, float pos_z, Ogre::Quaternion rot, Ogre::Vector3 scale)
{
    Json::Value jcolmesh;

    jcolmesh["name"]    = name;
    jcolmesh["pos_x"]   = pos_x;
    jcolmesh["pos_z"]   = pos_z;
    jcolmesh["scale_x"] = scale.x;
    jcolmesh["scale_y"] = scale.y;
    jcolmesh["scale_z"] = scale.z;
    jcolmesh["rotq_x"]  = rot.x;
    jcolmesh["rotq_y"]  = rot.y;
    jcolmesh["rotq_z"]  = rot.z;
    jcolmesh["rotq_w"]  = rot.w;

    m_json_root["collision_meshes"].append(jcolmesh);
}

void Terrn2Deployer::LoadCommonODefPlain(const char* full_path, const char* name_only)
{
    ODefParser parser;
    parser.Prepare();
    std::ifstream stream(full_path, std::ifstream::in);
    if (!stream.is_open())
    {
        LOGSTREAM << "ERROR: Failed to open file [" << full_path << "]";
        return;
    }
    char buffer[ODef::LINE_BUF_LEN];
    while (stream.good())
    {
        stream.getline(buffer, ODef::LINE_BUF_LEN);
        parser.ProcessLine(buffer);
    }
    if (stream.bad() || stream.fail())
    {
        LOGSTREAM << "ERROR reading file [" << full_path << "]";
        return;
    }
    m_common_odefs[name_only] = parser.Finalize();
}

void Terrn2Deployer::HandleException(const char* action)
{
    try { throw; } // Rethrow
    catch (Ogre::Exception& e)
    {
        LOGSTREAM << "Error " << action << ", message: " << e.getFullDescription();
    }
    catch (std::exception& e)
    {
        LOGSTREAM << "Error " << action << ", message: " << e.what();
    }
}


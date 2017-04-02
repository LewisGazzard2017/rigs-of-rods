
#pragma once

#include "ODefFileFormat.h"
#include "Terrn2FIleformat.h"
#include "TObjFileFormat.h"
#include "OTCFileformat.h"

#include <json/json.h>
#include <map>
#include <string>
#include <memory>

namespace RoR {

/// Reads all terrain definition files (terrn2, otc, tobj, odef...)
/// and composes a single JSON file called TJT (TerseJsonTerrain)
class Terrn2Deployer
{
public:
    bool                   DeployTerrn2(std::string terrn2_filename);

private:
    void                   LoadCommonODefPlain(const char* full_path, const char* name_only);
    bool                   CheckAndLoadOTC();
    void                   AddCollMeshJson(const char* name, float pos_x, float pos_z, Ogre::Quaternion rot, Ogre::Vector3 scale);
    void                   ProcessTerrn2Json();
    void                   ProcessTobjGrassJson();
    void                   ProcessTobjTreesJson();
    void                   ProcessTobjJson();
    void                   ProcessOtcJson();
    void                   HandleException(const char* action);

    std::vector<std::shared_ptr<TObjFile> >  m_tobj_list;
    std::shared_ptr<OTCFile>  m_otc;
    Terrn2Def                 m_terrn2;
    Json::Value               m_json_root;
};

} // namespace RoR

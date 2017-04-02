
#pragma once

#include "MiniZ.h"
#include "ODefFileFormat.h"
#include "Terrn2FIleformat.h"
#include "TObjFileFormat.h"
#include "OTCFileformat.h"

#include "json/json.h"
#include <map>
#include <string>
#include <memory>

namespace RoR {

typedef std::map<std::string, std::shared_ptr<ODefFile>> CommonOdefMap_t;

class Terrn2Deployer
{
public:
    bool                   DeployTerrn2(Terrn2Def& def, const char* zip_path);
    void                   LoadCommonODefs(const char* dir_path);

private:
    void                   ResetContext();
    void                   LoadCommonODefPlain(const char* full_path, const char* name_only);
    bool                   CheckAndLoadOTC();
    void                   AddCollMeshJson(const char* name, float pos_x, float pos_z, Ogre::Quaternion rot, Ogre::Vector3 scale);
    void                   ProcessTerrn2Json();
    void                   ProcessTobjGrassJson(std::vector<std::shared_ptr<TObjFile> >& tobj_list);
    void                   ProcessTobjTreesJson(std::vector<std::shared_ptr<TObjFile> >& tobj_list);
    void                   ProcessOtcJson();
    std::unique_ptr<void>  ExtractFileZip(const char* filename, size_t* len);

    CommonOdefMap_t           m_common_odefs;
    std::shared_ptr<OTCFile>  m_otc;
    mz_zip_archive            m_zip;
    Terrn2Def                 m_terrn2;
    std::string               m_zip_path;
    Json::Value               m_json_col_meshes;
    Json::Value               m_json_grass;
    Json::Value               m_json_forests;
    Json::Value               m_json_terrn2;
    Json::Value               m_json_otc;
};

} // namespace RoR

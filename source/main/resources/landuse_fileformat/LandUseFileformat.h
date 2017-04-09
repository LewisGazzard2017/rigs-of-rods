
#pragma once

#include <string>
#include <list>
#include <memory> // for shared_ptr

namespace RoR
{

struct LandUseEntry
{
    LandUseEntry(size_t color, std::string const & name)
        : rgba_color(color), groundmodel_name(name)
    {}

    size_t rgba_color;
    std::string groundmodel_name;
};

/// Definition of terrain's traction map
/// Specifies texture and maps colors to GroundModel-s
struct LandUseDef
{
    std::list<std::string> groundmodel_files;
    std::string texture_file;
    std::string default_groundmodel;
    std::list<LandUseEntry> mappings;
};

class LandUseParser
{
public:
    void LoadLandUseCfg(std::string const & filename);
    std::shared_ptr<LandUseDef>& GetLandUseDef() { return m_def; }

private:
    std::shared_ptr<LandUseDef> m_def;
};

} // namespace RoR

#include "LandUseFileformat.h"

#include "ConfigFile.h"

#include <OgreResourceGroupManager.h>
#include <OgreLogManager.h>

#define LOGSTREAM Ogre::LogManager::getSingleton().stream() << "[RoR|LandUse fileformat] "

void RoR::LandUseParser::LoadLandUseCfg(std::string const & filename)
{
    // Open the file
    std::string group;
    try
    {
        group = Ogre::ResourceGroupManager::getSingleton().findGroupContainingResource(filename);
    }
    catch (...)
    {} // The path may be global -> yields exception, but it's a valid case.

    RoR::ConfigFile cfg;
    if (group == "")
        cfg.loadDirect(filename);
    else
        cfg.loadFromResourceSystem(filename, group, "\x09:=", true);

    m_def = std::make_shared<LandUseDef>();

    // NOTE: This fileformat is truly awful - take the following code as reference ~ only_a_ptr, 04/2017

    Ogre::ConfigFile::SectionIterator section_itor = cfg.getSectionIterator();
    while (section_itor.hasMoreElements()) // Order matters
    {
        const std::string section_name = section_itor.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap& settings = *section_itor.getNext();
        if (section_name == "general" || section_name == "config")
        {
            for (auto& entry : settings) // Order matters
            {
                Ogre::StringUtil::trim(entry.second);
                if (entry.first == "texture")
                {
                    if (!entry.second.empty())
                        m_def->texture_file = entry.second;
                }
                else if (entry.first == "frictionconfig" || entry.first == "loadGroundModelsConfig")
                {
                    if (!entry.second.empty())
                        m_def->groundmodel_files.push_back(entry.second);
                }
                else if (entry.first == "defaultuse")
                {
                    m_def->default_groundmodel = entry.second;
                }
                else
                {
                    LOGSTREAM << "Invalid key '" << entry.first << "' in file '" << filename << "'";
                }
            }
        }
        else if (section_name == "use-map")
        {
            for (auto& entry : settings) // Order matters
            {
                if (entry.first.size() != 10)
                {
                    LOGSTREAM << "Invalid color'" << entry.first << "' in file '" << filename << "'";
                    continue;
                }
                char* ptr; // TODO: Check error
                size_t color = strtoul(entry.first.c_str(), &ptr, 16);
                m_def->mappings.push_back(LandUseEntry(color, entry.second));
            }
        }
    }
}
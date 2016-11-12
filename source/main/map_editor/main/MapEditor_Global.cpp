
#include "MapEditor_Global.h"

#include <string>

std::string MapEditor_GetDirectoryPath(std::string const & dir_in)
{
    size_t pos = dir_in.find_last_of("\\/");
    if (pos != std::string::npos)
    {
        return dir_in.substr(0, pos);
    }
    return "";
}

bool MapEditor_StrStartsWith(std::string haystack, std::string const & needle)
{
    Ogre::StringUtil::trim(haystack);
    return haystack.compare(0, needle.length(), needle) == 0;
}

/*
    This source file is part of Rigs of Rods
    Copyright 2016+ Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/** 
    @file
    @author Petr Ohlidal
    @date   10/2016
    @brief  Local database of installed content

    Terminology:

        "junk" are unrecognized files (shouldn't appear in given location at all).
        "duds" are files with valid name/extension but invalid/missing content.

    File structure ($ means GVar):

        $(sys_cache_dir)/mod-cache.json (file)
            ~ Cachefile. Lists all managed files + attributes.

        $(sys_user_dir)/vehicles (dir)
            ~ Directory for softbody actors (vehicles/machinery/other...)
              Recursive - you can put data in any subdirectory structure.
              Only ZIP archives are accepted. Other files are ignored as junk.
              The ZIPs must contain min. 1 truckfile, or they're ignored as duds.

        $(sys_user_dir)/terrains (dir)
            ~ Directory for terrains (.terrn/.terrn2)
              Recursive (see above), only ZIPs accepted (see above)
              The ZIPs must contain min. 1 .terrn[2] file, or they're ignored as duds.

        $(sys_user_dir)/load_all (dir)
            ~ Directory for misc content which needs to be loaded (shared content?)
              Non-recursive - accepts files on root level, descends to ZIPs and dirs.

    JSON structure:

        {
            "version": 1,

            "vehicles": {
                "/subdir/ExampleTrucks.zip": {
                    "timestamp": int,
                    "truckfiles": {
                        "example1.truck": {
                            "name": str,
                            "thumb_img_path": str,
                            "modules": array,

                            "driveable": int,
                            "engine_type": int,
                            "num_gears": int,
                        }
                    }
                }
            },

            "terrains": {
                "/subdir/ExampleTerrains.zip": {
                    "timestamp": int,
                    "terrains": {
                        "example1.terrn2": {
                            "name": str,
                            "filename": str,
                            "thumb_img_path": str,
                        }
                    }
                }
            },

            "load_all": {
                ... TODO ...
            }
        }
*/

#include <vector>
#include <string>
#include <map>

namespace RoR {
namespace ModCache {

enum State
{
    MODCACHE_STATE_NOT_INIT,
    MODCACHE_STATE_EMPTY,     //< No JSON and empty cache dir
    MODCACHE_STATE_CORRUPT,   //< Corrupt JSON or missing JSON and non-empty dir
    MODCACHE_STATE_UNCHECKED, //< Valid but needs check for updated content
    MODCACHE_STATE_DIRTY,     //< Checked, updates not processed
    MODCACHE_STATE_FORCED,    //< Forced rebuild
    MODCACHE_STATE_ERROR,     //< Error while processing
    MODCACHE_STATE_READY
};

struct Stats
{
    Stats();

    State initial_state;

    int purge_num_files;
    int purge_num_dirs;
    int purge_num_errors;

    int vehicles_num_junk;
    int vehicles_num_duds;
    int vehicles_num_zips;
    int vehicles_num_dirs;

    int terrains_num_junk;
    int terrains_num_duds;
    int terrains_num_zips;
    int terrains_num_dirs;
};

struct ProgressInfo
{
    ProgressInfo();

    static const int LABEL_LEN = 100;
    static const int INFO_LEN  = 300;

    char label[2][LABEL_LEN];
    char info [2][INFO_LEN];
    const char* title;
};

struct AuthorInfo
{
    std::string name;
    std::string email;
};

struct Entry
{
    /// These are the category numbers from the repository. do not modify them!
    enum Category
    {
        UNKNOWN             = 0,

        OTHER_LAND_VEHICLES = 108,

        STREET_CARS         = 146,
        LIGHT_RACING_CARS   = 147,
        OFFROAD_CARS        = 148,
        FANTASY_CARS        = 149,
        BIKES               = 150,
        CRAWLERS            = 155,

        TOWERCRANES         = 152,
        MOBILE_CRANES       = 153,
        OTHER_CRANES        = 154,

        BUSES               = 107,
        TRACTORS            = 151,
        FORKLIFTS           = 156,
        FANTASY_TRUCKS      = 159,
        TRANSPORT_TRUCKS    = 160,
        RACING_TRUCKS       = 161,
        OFFROAD_TRUCKS      = 162,

        BOATS               = 110,
        SUBMARINE           = 875,

        HELICOPTERS         = 113,
        AIRCRAFT            = 114,

        TRAILERS            = 117,
        OTHER_LOADS         = 118,
        CONTAINER           = 859,

        ADDON_TERRAINS      = 129,
        OFFICIAL_TERRAINS   = 5000, ///< Note: not in repository
        NIGHT_TERRAINS      = 5001, ///< Note: not in repository

        SPECIAL_UNSORTED    = 9990,
        SPECIAL_ALL         = 9991,
        SPECIAL_FRESH       = 9992,
        SPECIAL_HIDDEN      = 9993,
    };

    std::string             name;
    std::string             filename;
    std::string             description;
    std::string             thumb_img_name;
    Category                category;
    size_t                  added_timestamp;
    std::vector<AuthorInfo> authors;
};

struct SoftbodyEntry: public Entry
{
    std::list<std::string> modules;
    char                   engine_type;
    int                    driveable;
    int                    num_gears;
    std::string            file_ext;
};

struct TerrainEntry: public Entry
{
};

struct CategoryInfo // Required for old SelectorGUI
{
    CategoryInfo(Entry::Category c, std::string n): id(c), title(n) {}

    Entry::Category id;
    std::string title;
};

typedef std::map<Entry::Category, CategoryInfo> CategoryInfoMap; // Required for old SelectorGUI

void                         InitializeAsync(bool force_regen);
bool                         IsInitFinished();
ProgressInfo                 GetProgressInfo();
std::vector<SoftbodyEntry>*  GetSoftbodies();
std::vector<TerrainEntry>*   GetTerrains();
CategoryInfoMap&             GetCategories();

} // namespace ModCache
} // namespace RoR

#pragma once

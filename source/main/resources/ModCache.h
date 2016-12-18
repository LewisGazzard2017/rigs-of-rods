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

#pragma once

#include <vector>
#include <list>
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
        CATEGORY_UNKNOWN             = 0,

        CATEGORY_OTHER_LAND_VEHICLES = 108,

        CATEGORY_STREET_CARS         = 146,
        CATEGORY_LIGHT_RACING_CARS   = 147,
        CATEGORY_OFFROAD_CARS        = 148,
        CATEGORY_FANTASY_CARS        = 149,
        CATEGORY_BIKES               = 150,
        CATEGORY_CRAWLERS            = 155,

        CATEGORY_TOWERCRANES         = 152,
        CATEGORY_MOBILE_CRANES       = 153,
        CATEGORY_OTHER_CRANES        = 154,

        CATEGORY_BUSES               = 107,
        CATEGORY_TRACTORS            = 151,
        CATEGORY_FORKLIFTS           = 156,
        CATEGORY_FANTASY_TRUCKS      = 159,
        CATEGORY_TRANSPORT_TRUCKS    = 160,
        CATEGORY_RACING_TRUCKS       = 161,
        CATEGORY_OFFROAD_TRUCKS      = 162,

        CATEGORY_BOATS               = 110,
        CATEGORY_SUBMARINE           = 875,

        CATEGORY_HELICOPTERS         = 113,
        CATEGORY_AIRCRAFT            = 114,

        CATEGORY_TRAILERS            = 117,
        CATEGORY_OTHER_LOADS         = 118,
        CATEGORY_CONTAINER           = 859,

        CATEGORY_ADDON_TERRAINS      = 129,
        CATEGORY_OFFICIAL_TERRAINS   = 5000, ///< Note: not in repository
        CATEGORY_NIGHT_TERRAINS      = 5001, ///< Note: not in repository

        CATEGORY_SPECIAL_UNSORTED    = 9990,
        CATEGORY_SPECIAL_ALL         = 9991,
        CATEGORY_SPECIAL_FRESH       = 9992,
        CATEGORY_SPECIAL_HIDDEN      = 9993,
    };

    enum Type
    {
        TYPE_INVALID,
        TYPE_SOFTBODY,
        TYPE_TERRAIN
    };

    virtual ~Entry() {}
    virtual Type GetType() { return TYPE_INVALID; }

    std::string             name;
    std::string             filename;
    std::string             description;
    std::string             thumb_img_name;
    std::string             guid;
    Category                category;
    size_t                  added_timestamp;
    std::vector<AuthorInfo> authors;
};

struct SoftbodyEntry: public Entry
{
    Type GetType() override { return TYPE_SOFTBODY; }

    std::list<std::string> modules;
    char                   engine_type;
    int                    driveable;
    int                    num_gears;
    std::string            file_ext;
    int                    numeric_id;
};

struct TerrainEntry: public Entry
{
    Type GetType() override { return TYPE_TERRAIN; }
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

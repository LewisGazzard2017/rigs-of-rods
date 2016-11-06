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

#include "ModCache.h"

#include "Application.h"
#include "BeamData.h" // For authorinfo_t
#include "MiniZ.h"
#include "PlatformUtils.h"
#include "RigDef_Parser.h"
#include "TerrainManager.h"
#include "TinyDir.h"

#include <json/json.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <thread>
#include <mutex>
#include <cstdlib> // remove()
#include <cstring> // strrchr()
#include <atomic>

#define ZIP_FILENAME_BUF_LEN 200
#define CACHEFILE_NAME       "mod-cache.json"

#define LOGSTREAM Ogre::LogManager::getSingleton().stream() << "[RoR|ModCache] "

#define PROGINFO_SCOPE_LOCK() std::lock_guard<std::mutex> lock(s_prog_info_mutex);

namespace RoR {
namespace ModCache {

// ===== Declarations =====

enum InitState { INIT_PENDING, INIT_RUNNING, INIT_DONE, INIT_FAILED };

enum ZipType { ZIPTYPE_TRUCK, ZIPTYPE_TERRN };

// Init thread data
static Json::Value               s_json;
static Stats                     s_stats;
static State                     s_state;
static std::thread               s_init_thread;
static std::string               s_cache_dir;
static std::string               s_user_dir;

// Shared data
static InitState                 s_init_state;
static std::mutex                s_init_state_mutex;
static ProgressInfo              s_prog_info;
static std::mutex                s_prog_info_mutex;

// Main thread data
static bool                      s_init_finished;

void        AsyncInit                (bool force_regen);
void        RebuildFromScratch       ();
bool        FlushJson                ();
void        ProcessTruckfileBuffer   (Json::Value& json, void* data, size_t len);
void        ProcessTerrn2Buffer      (Json::Value &json, void* data, size_t len);
void        PurgeCacheDir            ();
void        LoadJsonCacheFile        ();
void        SetInitState             (InitState s);
InitState   GetInitState             ();

// ===== Public functions =====

Stats::Stats()
{
    memset(this, 0, sizeof(Stats));
}

ProgressInfo::ProgressInfo()
{
    memset(this, 0, sizeof(ProgressInfo));
}

bool IsInitFinished()
{
    if (s_init_finished)
    {
        return true;
    }

    InitState state = GetInitState();
    if (state == INIT_DONE || state == INIT_FAILED)
    {
        s_init_finished = true;
    }
    return s_init_finished;
}

void InitializeAsync(bool force_regen)
{
    if (s_init_state != INIT_PENDING)
    {
        return;
    }

    s_cache_dir = App::GetSysUserDir() + PATH_SLASH + "cache2016"; // Test!
    //s_cache_dir = App::GetSysCacheDir();
    s_user_dir = App::GetSysUserDir();
    s_init_thread = std::thread(AsyncInit, force_regen);
}

ProgressInfo GetProgressInfo()
{
    PROGINFO_SCOPE_LOCK();
    return s_prog_info;
}

// ===== File extension tests (the MacroMadness) =====

// Test one (C)haracter
#define CTEST(_OFFS_, _CHAR_)               (filename[len - (_OFFS_) - 1] == (_CHAR_))
// Test one (P)osition
#define PTEST(_OFFS_, _CHAR_a_, _CHAR_A_)   (CTEST((_OFFS_), (_CHAR_a_)) || CTEST((_OFFS_), (_CHAR_A_)))
// Test letters
#define Dot(_OFFS_)                         (CTEST((_OFFS_),'.'))
#define A(_OFFS_)                           (PTEST((_OFFS_),'a','A'))
#define B(_OFFS_)                           (PTEST((_OFFS_),'b','B'))
#define C(_OFFS_)                           (PTEST((_OFFS_),'c','C'))
#define D(_OFFS_)                           (PTEST((_OFFS_),'d','D'))
#define E(_OFFS_)                           (PTEST((_OFFS_),'e','E'))
#define F(_OFFS_)                           (PTEST((_OFFS_),'f','F'))
#define H(_OFFS_)                           (PTEST((_OFFS_),'h','H'))
#define I(_OFFS_)                           (PTEST((_OFFS_),'i','I'))
#define K(_OFFS_)                           (PTEST((_OFFS_),'k','K'))
#define L(_OFFS_)                           (PTEST((_OFFS_),'l','L'))
#define M(_OFFS_)                           (PTEST((_OFFS_),'m','M'))
#define N(_OFFS_)                           (PTEST((_OFFS_),'n','N'))
#define O(_OFFS_)                           (PTEST((_OFFS_),'o','O'))
#define P(_OFFS_)                           (PTEST((_OFFS_),'p','P'))
#define R(_OFFS_)                           (PTEST((_OFFS_),'r','R'))
#define T(_OFFS_)                           (PTEST((_OFFS_),'t','T'))
#define U(_OFFS_)                           (PTEST((_OFFS_),'u','U'))
#define X(_OFFS_)                           (PTEST((_OFFS_),'x','X'))

#define MK_CHECK_EXT_FN(_EXT_, _BODY_)  inline bool CheckExt##_EXT_(const char* filename, unsigned len) { _BODY_ }

// Length includes dot. Example: ".truck" = 6 chars
MK_CHECK_EXT_FN(Truck,   {return (len > 6) && Dot(5) && T(4) && R(3) && U(2) && C(1) && K(0)                        ;})
MK_CHECK_EXT_FN(Car,     {return (len > 4) && Dot(3) && C(2) && A(1) && R(0)                                        ;})
MK_CHECK_EXT_FN(Airplane,{return (len > 9) && Dot(8) && A(7) && I(6) && R(5) && P(4) && L(3) && A(2) && N(1) && E(0);})
MK_CHECK_EXT_FN(Boat,    {return (len > 5) && Dot(4) && B(3) && O(2) && A(1) && T(0)                                ;})
MK_CHECK_EXT_FN(Machine, {return (len > 8) && Dot(7) && M(6) && A(5) && C(4) && H(3) && I(2) && N(1) && E(0)        ;})
MK_CHECK_EXT_FN(Fixed,   {return (len > 6) && Dot(5) && F(4) && I(3) && X(2) && E(1) && D(0)                        ;})
MK_CHECK_EXT_FN(Load,    {return (len > 5) && Dot(4) && L(3) && O(2) && A(1) && D(0)                                ;})
MK_CHECK_EXT_FN(Terrn2,  {return (len > 7) && Dot(6) && T(5) && E(4) && R(3) && R(2) && N(1) && (filename[0] == '2');})

inline bool CheckExtZip(const char* ext)
{
    return (ext[0] != 0 && (ext[0] == 'z' || ext[0] == 'Z'))
        && (ext[1] != 0 && (ext[1] == 'i' || ext[1] == 'I'))
        && (ext[2] != 0 && (ext[2] == 'p' || ext[2] == 'P'));
}

inline bool IsSpecialDir(const char* name)
{
    return (name[0] == '.') && ((name[1] == '\0') || ((name[1] == '.') && (name[2] == '\0')));
}

// ===== Private functions =====

void AsyncInit(bool force_regen)
{
    SetInitState( INIT_RUNNING);
    if (force_regen)
    {
        LOGSTREAM << "Performing full rebuild (forced).";
        s_stats.initial_state = MODCACHE_STATE_FORCED;
        PurgeCacheDir();
        RebuildFromScratch();
        s_state = MODCACHE_STATE_READY;
        return;
    }

    LoadJsonCacheFile();
    s_stats.initial_state = s_state;

    if (s_state == MODCACHE_STATE_EMPTY)
    {
        LOGSTREAM << "Detected as empty. Will perform full rebuild.";
        RebuildFromScratch();
        s_state = MODCACHE_STATE_READY;
    }
    else if (s_state == MODCACHE_STATE_CORRUPT)
    {
        LOGSTREAM << "Detected as corrupt (cachefile missing or invalid). "
                  << "Will purge and perform full rebuild.";
        PurgeCacheDir();
        RebuildFromScratch();
        s_state = MODCACHE_STATE_READY;
    }
    else if (s_state == MODCACHE_STATE_UNCHECKED)
    {
        LOGSTREAM << "Detected as valid. Will check for updates.";
    }
    else
    {
        LOGSTREAM << "FATAL: Invalid state: [" << s_state << "]. Exit.";
        SetInitState( INIT_FAILED);
        s_state = MODCACHE_STATE_NOT_INIT;
    }
}

void SetInitState(InitState s)
{
    std::lock_guard<std::mutex> lock(s_init_state_mutex);
    s_init_state = s;
}

InitState GetInitState()
{
    std::lock_guard<std::mutex> lock(s_init_state_mutex);
    return s_init_state;
}

void LoadJsonCacheFile()
{
    {
        PROGINFO_SCOPE_LOCK()
        s_prog_info.title = "[ModCache] Detecting cache state ...";
    }
    std::string cachefile_path = s_cache_dir + PATH_SLASH + CACHEFILE_NAME;
    if (! PlatformUtils::FileExists(cachefile_path))
    {
        tinydir_dir dir;
        if (tinydir_open(&dir, App::GetSysCacheDir().c_str()) != 0)
        {
            s_state = MODCACHE_STATE_EMPTY; // Directory doesn't even exist.
        }
        else
        {
            s_state = (dir.n_files == 0) ? MODCACHE_STATE_EMPTY : MODCACHE_STATE_CORRUPT;
        }
        tinydir_close(&dir);
        return;
    }

    std::fstream f(cachefile_path);
    if (! f.is_open())
    {
        s_state = MODCACHE_STATE_CORRUPT;
        return;
    }

    Json::Value json;
    std::string parse_err;
    if (! Json::parseFromStream(Json::CharReaderBuilder(), f, &json, &parse_err))
    {
        s_state = MODCACHE_STATE_CORRUPT;
        return;
    }

    s_state = MODCACHE_STATE_UNCHECKED;
}

void PurgeCacheDir()
{
    ProgressInfo info;
    info.title = "[ModCache] Purging cache directory ...";
    s_prog_info = info;

    tinydir_dir dir;
    tinydir_file file;
    tinydir_open(&dir, s_cache_dir.c_str());

    for (unsigned i = 0; i < dir.n_files; ++i)
    {
        tinydir_readfile(&dir, &file);
        const bool is_dir = (file.is_dir == 1);
        int result = remove(file.path);
        if (result != 0)
        {
            LOGSTREAM << "ERROR: Failed to purge file [" << file.path << "]; result code: [" << result << "]";
            ++s_stats.purge_num_errors;
        }
        else
        {
            if (is_dir) { ++s_stats.purge_num_dirs;  }
            else        { ++s_stats.purge_num_files; }
        }
    }

    tinydir_close(&dir);
}

const char* GetZipfileLocalPath(const char* full_path, int subdir_depth)
{
    const char* local_path = full_path;
    for (int i = 0; i <= subdir_depth; ++i)
    {
        local_path = strrchr(local_path, '/'); // Tinydir always returns paths with '/'
    }
    return local_path;
}

void StatsAddDud(ZipType zip_type)
{
    if (zip_type == ZIPTYPE_TRUCK)
        ++s_stats.vehicles_num_duds;
    else
        ++s_stats.terrains_num_duds;
}

void StatsAddZip(ZipType zip_type)
{
    if (zip_type == ZIPTYPE_TRUCK)
        ++s_stats.vehicles_num_zips;
    else
        ++s_stats.terrains_num_zips;
}

void ProcessNewZip(const char* path, int subdir_depth, ZipType zip_type)
{
    const char* zip_local_path = GetZipfileLocalPath(path, subdir_depth);
    {
        PROGINFO_SCOPE_LOCK()
        sprintf(s_prog_info.label[0], "Package:");
        sprintf(s_prog_info.info[0], "%s", zip_local_path);
    }

    mz_zip_archive zip;
    memset(&zip, 0, sizeof(zip));

    if (mz_zip_reader_init_file(&zip, path, 0) == MZ_FALSE)
    {
        StatsAddDud(zip_type);
        mz_zip_reader_end(&zip);
        return;
    }

    int num_files = (int)mz_zip_reader_get_num_files(&zip);
    if (num_files == 0)
    {
        StatsAddDud(zip_type);
        mz_zip_reader_end(&zip);
        return;
    }

    Json::Value json_zip; // Defaults to null
    for (int i = 0; i < num_files; ++i)
    {
        char filename[ZIP_FILENAME_BUF_LEN] = "";
        unsigned num_chars = mz_zip_reader_get_filename(&zip, i, filename, ZIP_FILENAME_BUF_LEN) - 1;

        if (zip_type == ZIPTYPE_TRUCK &&
            !CheckExtTruck    ( filename, num_chars) &&
            !CheckExtCar      ( filename, num_chars) &&
            !CheckExtAirplane ( filename, num_chars) &&
            !CheckExtBoat     ( filename, num_chars) &&
            !CheckExtMachine  ( filename, num_chars) &&
            !CheckExtLoad     ( filename, num_chars))
        {
            continue;
        }
        
        if (zip_type == ZIPTYPE_TERRN && !CheckExtTerrn2(filename, num_chars))
        {
            continue;
        }

        size_t data_size = 0;
        void* raw_data = mz_zip_reader_extract_to_heap(&zip, i, &data_size, 0);
        if (raw_data == nullptr)
        {
            LOGSTREAM << "ERROR: Failed to uncompress file [" << filename << "] from archive [" << path << "]";
            continue;
        }

        {
            PROGINFO_SCOPE_LOCK()
            sprintf(s_prog_info.label[1], "File:");
            sprintf(s_prog_info.info[1], "%s", filename);
        }
        
        Json::Value json_def = Json::objectValue;
        if (zip_type == ZIPTYPE_TRUCK)
        {
            ProcessTruckfileBuffer(json_def, raw_data, data_size);
        }
        else
        {
            ProcessTerrn2Buffer(json_def, raw_data, data_size);
        }

        if (json_zip.isNull())
        {
            json_zip = Json::objectValue;
        }
        json_zip[filename] = json_def;
    }

    mz_zip_reader_end(&zip);

    if (json_zip.isNull())
    {
        StatsAddDud(zip_type);
    }
    else
    {
        const char* zip_type_str = (zip_type == ZIPTYPE_TERRN) ? "terrains" : "vehicles";
        s_json[zip_type_str][zip_local_path] = json_zip;
        StatsAddZip(zip_type);
    }

    {
        PROGINFO_SCOPE_LOCK()
        s_prog_info.label[1][0] = 0; // Only erase line1, line0 won't last long...
        s_prog_info.info [1][0] = 0;
    }
}

void ProcessTerrn2Buffer(Json::Value &json, void* data, size_t len)
{
    TerrainManager tm;
    Ogre::DataStreamPtr ds = Ogre::DataStreamPtr(OGRE_NEW Ogre::MemoryDataStream(data, len));
    tm.loadTerrainConfigBasics(ds);

    json["authors"] = Json::arrayValue;
    int i = 0;
    auto & authors = tm.GetAuthors();
    auto itor_end = authors.end();
    for (auto itor = authors.begin(); itor != itor_end; ++itor)
    {
        Json::Value author;
        author["id"]    = itor->id;
        author["email"] = itor->email;
        author["name"]  = itor->name;
        author["type"]  = itor->type;
        json["authors"][i] = author;
        ++i;
    }

    json["name"]        = tm.getTerrainName();
    json["category_id"] = tm.getCategoryID();
    json["guid"]        = tm.getGUID();
    json["version"]     = tm.getVersion();
}

void ProcessTruckfileBuffer(Json::Value& json_def, void* raw_data, size_t len)
{
    std::vector<const char*> lines;
    lines.reserve(5000);

    char* data = (char*)raw_data;
    size_t pos = 0;
    const char* line_start = data;
    for (;;)
    {
        if (data[pos] == '\n')
        {
            data[pos] = 0;
            if (pos != 0 && data[pos - 1] == '\r')
            {
                data[pos - 1] = 0;
            }
            lines.push_back(line_start);
            ++pos;
            if (pos == len)
            {
                break;
            }
            line_start = data + pos;
        }
        else
        {
            ++pos;
            if (pos == len)
            {
                break;
            }
        }
    }

    RigDef::Parser parser;
    parser.Prepare();
    for (const char* line : lines)
    {
        parser.ProcessRawLine(line);
    }
    parser.Finalize();
    auto rig_def = parser.GetFile();

    json_def["name"] = rig_def->name; // More to come...
}

void RebuildZipsRecursive(std::string dir_path, int depth, ZipType zip_type)
{
    tinydir_dir dir;       memset(&dir,  0, sizeof(tinydir_dir ));
    tinydir_file file;     memset(&file, 0, sizeof(tinydir_file));

    tinydir_open(&dir, dir_path.c_str());

    while (dir.has_next)
    {
        tinydir_readfile(&dir, &file);
        if (file.is_dir)
        {
            if (! IsSpecialDir(file.name))
            {
                ++s_stats.vehicles_num_dirs;
                RebuildZipsRecursive(dir_path + PATH_SLASH + file.name, depth + 1, zip_type);
            }
        }
        else if (file.is_reg && CheckExtZip(file.extension))
        {
            ProcessNewZip(file.path, depth, zip_type);
        }
        else
        {
            ++s_stats.vehicles_num_junk;
        }
        tinydir_next(&dir);
    }
}

bool FlushJson()
{
    std::string cachefile_path = s_cache_dir + PATH_SLASH + CACHEFILE_NAME;
    std::ofstream f;
    f.open(cachefile_path, std::fstream::out);
    if (! f.is_open())
    {
        LOGSTREAM << "ERROR: Failed to open cachefile for writing: [" << cachefile_path << "]";
        return false;
    }

    Json::StreamWriterBuilder b;
    Json::StreamWriter* w = b.newStreamWriter();
    int result = w->write(s_json, &f);

    if (result != 0 || f.fail())
    {
        LOGSTREAM << "ERROR: Failed to write down cachefile: [" << cachefile_path << "]";
        return false;
    }

    return true;
}

void RebuildFromScratch()
{
    s_json = Json::objectValue;
    s_json["version"]  = 1;
    s_json["vehicles"] = Json::objectValue;
    s_json["terrains"] = Json::objectValue;
    s_json["load_all"] = Json::objectValue;

    {
        PROGINFO_SCOPE_LOCK()
        s_prog_info.title = "[ModCache] Processing vehicle packages ...";
    }

    RebuildZipsRecursive(s_user_dir + PATH_SLASH + "vehicles", 0, ZIPTYPE_TRUCK);

    {
        PROGINFO_SCOPE_LOCK()
        s_prog_info.title = "[ModCache] Processing terrain packages ...";
    }

    RebuildZipsRecursive(s_user_dir + PATH_SLASH + "terrains", 0, ZIPTYPE_TERRN);

    {
        PROGINFO_SCOPE_LOCK()
        s_prog_info.title = "[ModCache] Writing cachefile ...";
        s_prog_info.label[0][0] = 0;
        s_prog_info.label[1][0] = 0;
        s_prog_info.info [0][0] = 0;
        s_prog_info.info [1][0] = 0;
    }

    s_state = FlushJson() ? MODCACHE_STATE_READY : MODCACHE_STATE_ERROR;
    SetInitState(INIT_DONE);
}

} // namespace ModCache
} // namespace RoR

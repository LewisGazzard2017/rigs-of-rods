/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013-2014 Petr Ohlidal

    For more information, see http://www.rigsofrods.org/

    Rigs of Rods is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3, as
    published by the Free Software Foundation.

    Rigs of Rods is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/// @file   CacheSystem.h
/// @author Thomas Fischer
/// @date   21th of May 2008

#include "CacheSystem.h"

#include <OgreFileSystem.h>

#include "Application.h"
#include "BeamData.h"
#include "BeamEngine.h"
#include "ErrorUtils.h"
#include "GUIManager.h"
#include "ImprovedConfigFile.h"
#include "Language.h"
#include "PlatformUtils.h"
#include "RigDef_Parser.h"
#include "Settings.h"
#include "SHA1.h"
#include "SoundScriptManager.h"
#include "TerrainManager.h"
#include "Utils.h"

#ifdef USE_MYGUI
#include "GUI_LoadingWindow.h"
#endif // USE_MYGUI

using namespace Ogre;

// default constructor resets the data.
CacheEntry::CacheEntry() :
    //authors
    addtimestamp(0),
    beamcount(0),
    categoryid(0),
    categoryname(""),
    changedornew(false),
    commandscount(0),
    custom_particles(false),
    customtach(false),
    deleted(false),
    description(""),
    dirname(""),
    dname(""),
    driveable(0),
    enginetype('t'),
    exhaustscount(0),
    fext(""),
    filecachename(""),
    fileformatversion(0),
    filetime(0),
    fixescount(0),
    flarescount(0),
    flexbodiescount(0),
    fname(""),
    fname_without_uid(""),
    forwardcommands(false),
    hasSubmeshs(false),
    hash(""),
    hydroscount(0),
    importcommands(false),
    loadmass(0),
    managedmaterialscount(0),
    materialflarebindingscount(0),
    materials(),
    maxrpm(0),
    minitype(""),
    minrpm(0),
    nodecount(0),
    number(0),
    numgears(0),
    propscount(0),
    propwheelcount(0),
    rescuer(false),
    resourceLoaded(false),
    rollon(false),
    rotatorscount(0),
    shockcount(0),
    soundsourcescount(0),
    tags(""),
    torque(0),
    truckmass(0),
    turbojetcount(0),
    turbopropscount(0),
    type(""),
    uniqueid(""),
    usagecounter(0),
    version(0),
    wheelcount(0),
    wingscount(0)
{
    // driveable = 0 = NOT_DRIVEABLE
    // enginetype = t = truck is default
}

CacheSystem::CacheSystem() :
    changedFiles(0)
    , deletedFiles(0)
    , newFiles(0)
    , rgcounter(0)
{
    // register the extensions
    known_extensions.push_back("machine");
    known_extensions.push_back("fixed");
    known_extensions.push_back("terrn2");
    known_extensions.push_back("truck");
    known_extensions.push_back("car");
    known_extensions.push_back("boat");
    known_extensions.push_back("airplane");
    known_extensions.push_back("trailer");
    known_extensions.push_back("load");
    known_extensions.push_back("train");
}

CacheSystem::~CacheSystem()
{
}

void CacheSystem::setLocation(String cachepath, String configpath)
{
    location = cachepath;
    configlocation = configpath;
}

void CacheSystem::Startup(bool force_check)
{
    if (BSETTING("NOCACHE", false))
    {
        LOG("Cache disabled via command line switch");
        return;
    }

    // read valid categories from file
    readCategoryTitles();

    // calculate sha1 over all the content
    currentSHA1 = filenamesSHA1();

    CacheValidityState validity = CACHE_STATE_UNKNOWN;
    if (force_check)
    {
        validity = CACHE_NEEDS_UPDATE_INCREMENTAL;
    }
    else
    {
        validity = IsCacheValid();
    }

    if (validity != CACHE_VALID)
    {
        LOG("cache invalid, updating ...");
        // generate the cache
        generateCache(validity == CACHE_NEEDS_UPDATE_FULL);

        LOG("Cache updated, enumerating all resource groups...");
        StringVector sv = ResourceGroupManager::getSingleton().getResourceGroups();
        for (auto itor = sv.begin(); itor != sv.end(); ++itor)
        {
            LOG("\t\t" + *itor);
        }
    }

    LOG("loading cache...");
    // load the cache finally!
    loadCache();

    // show error on zero content
    if (entries.empty())
    {
        ErrorUtils::ShowError(_L("No content installed"), _L("You have no content installed"));
        exit(1337);
    }

    LOG("Cache loaded, enumerating all resource groups...");
    StringVector sv = ResourceGroupManager::getSingleton().getResourceGroups();
    for (auto itor = sv.begin(); itor != sv.end(); ++itor)
    {
        LOG("\t\t" + *itor);
    }

    LOG("cache loaded!");
}




String CacheSystem::getCacheConfigFilename(bool full)
{
    if (full)
        return location + String(CACHE_FILE);
    return String(CACHE_FILE);
}

// we implement this on our own, since we cannot reply on the ogre version
bool CacheSystem::resourceExistsInAllGroups(Ogre::String filename)
{
    try
    {
        String group = ResourceGroupManager::getSingleton().findGroupContainingResource(filename);
        return !group.empty();
    }
    catch (...)
    {
        return false;
    }
}

CacheSystem::CacheValidityState CacheSystem::IsCacheValid()
{
    String cfgfilename = getCacheConfigFilename(false);
    ImprovedConfigFile cfg;
    if (!resourceExistsInAllGroups(cfgfilename))
    {
        LOG("unable to load config file: "+cfgfilename);
        return CACHE_NEEDS_UPDATE_FULL;
    }

    String group = ResourceGroupManager::getSingleton().findGroupContainingResource(cfgfilename);
    DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(cfgfilename, group);
    cfg.load(stream, "\t:=", false);
    String shaone = cfg.GetString("shaone");
    String cacheformat = cfg.GetString("cacheformat");

    if (shaone == "" || shaone != currentSHA1)
    {
        LOG("* mod cache is invalid (not up to date), regenerating new one ...");
        return CACHE_NEEDS_UPDATE_INCREMENTAL;
    }
    if (cacheformat != String(CACHE_FILE_FORMAT))
    {
        entries.clear();
        LOG("* mod cache has invalid format, trying to regenerate");
        return CACHE_NEEDS_UPDATE_INCREMENTAL;
    }
    LOG("* mod cache is valid, using it.");
    return CACHE_VALID;
}





bool CacheSystem::loadCache()
{
    // Clear existing entries
    entries.clear();

    String cfgfilename = getCacheConfigFilename(false);

    if (!resourceExistsInAllGroups(cfgfilename))
    {
        LOG("unable to load config file: "+cfgfilename);
        return false;
    }

    String group = ResourceGroupManager::getSingleton().findGroupContainingResource(String(cfgfilename));
    DataStreamPtr stream = ResourceGroupManager::getSingleton().openResource(cfgfilename, group);

    LOG("CacheSystem::loadCache");

    CacheEntry t;
    String line = "";
    int mode = 0;

    while (!stream->eof())
    {
        line = stream->getLine();

        // Ignore blanks & comments
        if (line.empty() || line.substr(0, 2) == "//")
        {
            continue;
        }

        // Skip these
        if (StringUtil::startsWith(line, "shaone=") || StringUtil::startsWith(line, "modcount=") || StringUtil::startsWith(line, "cacheformat="))
        {
            continue;
        }

        if (mode == 0)
        {
            // No current entry
            if (line == "mod")
            {
                mode = 1;
                t = CacheEntry();
                t.resourceLoaded = false;
                t.deleted = false;
                t.changedornew = false; // default upon loading
                // Skip to and over next {
                stream->skipLine("{");
            }
        }
        else if (mode == 1)
        {
            // Already in mod
            if (line == "}")
            {
                // Finished
                if (!t.deleted)
                {
                    entries.push_back(t);
                }
                mode = 0;
            }
            else
            {
                parseModAttribute(line, t);
            }
        }
    }
    return true;
}

String CacheSystem::getRealPath(String path)
{
    // this shall convert the path names to fit the operating system's flavor
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    // this is required for windows since we are case insensitive ...
    path = StringUtil::replaceAll(path, "/", "\\");
    StringUtil::toLowerCase(path);
#endif
    return path;
}

String CacheSystem::getVirtualPath(String path)
{
    path = StringUtil::replaceAll(path, "\\", "/");
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    // this is required for windows since we are case insensitive ...
    StringUtil::toLowerCase(path);
#endif
    return path;
}

int CacheSystem::incrementalCacheUpdate()
{
    entries.clear();

    if (!loadCache())
    //error loading cache!
        return -1;

    LOG("* incremental check starting ...");
    LOG("* incremental check (1/5): deleted and changed files ...");
#ifdef USE_MYGUI
    auto* loading_win = RoR::App::GetGuiManager()->GetLoadingWindow();
    loading_win->setProgress(20, _L("incremental check: deleted and changed files"));
#endif //USE_MYGUI
    std::vector<CacheEntry> changed_entries;
    UTFString tmp = "";
    String fn = "";
    int counter = 0;
    for (std::vector<CacheEntry>::iterator it = entries.begin(); it != entries.end(); it++ , counter++)
    {
#ifdef USE_MYGUI
        int progress = ((float)counter / (float)(entries.size())) * 100;
        tmp = _L("incremental check: deleted and changed files\n") + ANSI_TO_UTF(it->type) + _L(": ") + ANSI_TO_UTF(it->fname);
        loading_win->setProgress(progress, tmp);
#endif //USE_MYGUI
        // check whether the file exists
        if (it->type == "Zip")
            fn = getRealPath(it->dirname);
        else if (it->type == "FileSystem")
            fn = getRealPath(it->dirname + "/" + it->fname);

        if ((it->type == "FileSystem" || it->type == "Zip") && ! RoR::PlatformUtils::FileExists(fn.c_str()))
        {
            LOG("- "+fn+" is not existing");
#ifdef USE_MYGUI
            tmp = _L("incremental check: deleted and changed files\n") + ANSI_TO_UTF(it->fname) + _L(" not existing");
            loading_win->setProgress(20, tmp);
#endif //USE_MYGUI
            removeFileFromFileCache(it);
            it->deleted = true;
            // do not try: entries.erase(it)
            deletedFiles++;
            continue;
        }
        // check whether it changed
        if (it->type == "Zip")
        {
            // check file time, if that fails, fall back to sha1 (needed for platforms where filetime is not yet implemented!
            bool check = false;
            std::time_t ft = fileTime(fn);
            if (!ft)
            {
                // slow sha1 check
                char hash[256] = {};

                RoR::CSHA1 sha1;
                sha1.HashFile(const_cast<char*>(fn.c_str()));
                sha1.Final();
                sha1.ReportHash(hash, RoR::CSHA1::REPORT_HEX_SHORT);
                check = (it->hash != String(hash));
            }
            else
            {
                // faster file time check
                check = (it->filetime != ft);
            }

            if (check)
            {
                changedFiles++;
                LOG("- "+fn+" changed");
                it->changedornew = true;
                it->deleted = true; // see below
                changed_entries.push_back(*it);
            }
        }
    }

    // we try to reload one zip only one time, not multiple times if it contains more resources at once
    std::vector<Ogre::String> reloaded_zips;
    LOG("* incremental check (2/5): processing changed zips ...");
#ifdef USE_MYGUI
    loading_win->setProgress(40, _L("incremental check: processing changed zips\n"));
#endif //USE_MYGUI
    for (std::vector<CacheEntry>::iterator it = changed_entries.begin(); it != changed_entries.end(); it++)
    {
        bool found = false;
        for (std::vector<Ogre::String>::iterator it2 = reloaded_zips.begin(); it2 != reloaded_zips.end(); it2++)
        {
            if (*it2 == it->dirname)
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
#ifdef USE_MYGUI
            loading_win->setProgress(40, _L("incremental check: processing changed zips\n") + it->fname);
#endif //USE_MYGUI
            loadSingleZip(*it);
            reloaded_zips.push_back(it->dirname);
        }
    }
    LOG("* incremental check (3/5): new content ...");
#ifdef USE_MYGUI
    loading_win->setProgress(60, _L("incremental check: new content\n"));
#endif //USE_MYGUI
    checkForNewContent();

    LOG("* incremental check (4/5): new files ...");
#ifdef USE_MYGUI
    loading_win->setProgress(80, _L("incremental check: new files\n"));
#endif //USE_MYGUI
    checkForNewKnownFiles();

    LOG("* incremental check (5/5): duplicates ...");
#ifdef USE_MYGUI
    loading_win->setProgress(90, _L("incremental check: duplicates\n"));
#endif //USE_MYGUI
    for (std::vector<CacheEntry>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (it->deleted)
            continue;
        for (std::vector<CacheEntry>::iterator it2 = entries.begin(); it2 != entries.end(); it2++)
        {
            if (it2->deleted)
                continue;
            // clean paths, important since we compare them ...
            String basename, basepath;

            String dira = it->dirname;
            StringUtil::toLowerCase(dira);
            StringUtil::splitFilename(dira, basename, basepath);
            basepath = getVirtualPath(basepath);
            dira = basepath + basename;

            String dirb = it2->dirname;
            StringUtil::toLowerCase(dirb);
            StringUtil::splitFilename(dira, basename, basepath);
            basepath = getVirtualPath(basepath);
            dirb = basepath + basename;

            String dnameA = it->dname;
            StringUtil::toLowerCase(dnameA);
            StringUtil::trim(dnameA);
            String dnameB = it2->dname;
            StringUtil::toLowerCase(dnameB);
            StringUtil::trim(dnameB);

            String filenameA = it->fname;
            StringUtil::toLowerCase(filenameA);
            String filenameB = it2->fname;
            StringUtil::toLowerCase(filenameB);

            String filenameWUIDA = it->fname_without_uid;
            StringUtil::toLowerCase(filenameWUIDA);
            String filenameWUIDB = it2->fname_without_uid;
            StringUtil::toLowerCase(filenameWUIDB);

            // hard duplicate
            if (dira == dirb && dnameA == dnameB && filenameA == filenameB)
            {
                if (it->number == it2->number)
                    continue; // do not delete self
                LOG("- "+ it2->dirname+"/" + it->fname + " hard duplicate");
                it2->deleted = true;
                continue;
            }
            // soft duplicates
            else if (dira == dirb && dnameA == dnameB && filenameWUIDA == filenameWUIDB)
            {
                if (it->number == it2->number)
                    continue; // do not delete self
                LOG("- "+ it2->dirname+"/" + it->fname + " soft duplicate, resolving ...");
                // create sha1 and see whats the correct entry :)
                RoR::CSHA1 sha1;
                sha1.HashFile(const_cast<char*>(it2->dirname.c_str()));
                sha1.Final();
                char hashres[256] = "";
                sha1.ReportHash(hashres, RoR::CSHA1::REPORT_HEX_SHORT);
                String hashstr = String(hashres);
                if (hashstr == it->hash)
                {
                    LOG("  - entry 2 removed");
                    it2->deleted = true;
                }
                else if (hashstr == it2->hash)
                {
                    LOG("  - entry 1 removed");
                    it->deleted = true;
                }
                else
                {
                    LOG("  - entry 1 and 2 removed");
                    it->deleted = true;
                    it2->deleted = true;
                }
            }
        }
    }
#ifdef USE_MYGUI
    loading_win->setAutotrack(_L("loading...\n"));
#endif //USE_MYGUI

    this->writeGeneratedCache();

#ifdef USE_MYGUI
    RoR::App::GetGuiManager()->SetVisible_LoadingWindow(false);
#endif //USE_MYGUI
    LOG("* incremental check done.");
    return 0;
}

CacheEntry* CacheSystem::getEntry(int modid)
{
    for (std::vector<CacheEntry>::iterator it = entries.begin(); it != entries.end(); it++)
    {
        if (modid == it->number)
            return &(*it);
    }
    return 0;
}

void CacheSystem::generateCache(bool forcefull)
{
    this->modcounter = 0;

    // see if we can avoid a full regeneration
    if (forcefull || incrementalCacheUpdate())
    {
        loadAllZips();

        writeGeneratedCache();
    }
}

Ogre::String CacheSystem::formatInnerEntry(int counter, CacheEntry t)
{
    String result = "";
    result += "\tnumber=" + TOSTRING(counter) + "\n"; // always count linear!
    result += "\tdeleted=" + TOSTRING(t.deleted) + "\n";
    if (!t.deleted)
    {
        // this ensures that we wont break the format with empty ("") values
        if (t.minitype.empty())
            t.minitype = "unknown";
        if (t.type.empty())
            t.type = "unknown";
        if (t.dirname.empty())
            t.dirname = "unknown";
        if (t.fname.empty())
            t.fname = "unknown";
        if (t.fext.empty())
            t.fext = "unknown";
        if (t.dname.empty())
            t.dname = "unknown";
        if (t.hash.empty())
            t.hash = "none";
        if (t.uniqueid.empty())
            t.uniqueid = "no-uid";
        if (t.guid.empty())
            t.guid = "no-guid";
        if (t.fname_without_uid.empty())
            t.fname_without_uid = "unknown";
        if (t.filecachename.empty())
            t.filecachename = "none";

        result += "\tusagecounter=" + TOSTRING(t.usagecounter) + "\n";
        result += "\taddtimestamp=" + TOSTRING(t.addtimestamp) + "\n";
        result += "\tminitype=" + t.minitype + "\n";
        result += "\ttype=" + t.type + "\n";
        result += "\tdirname=" + t.dirname + "\n";
        result += "\tfname=" + t.fname + "\n";
        result += "\tfname_without_uid=" + t.fname_without_uid + "\n";
        result += "\tfext=" + t.fext + "\n";
        result += "\tfiletime=" + TOSTRING((long)t.filetime) + "\n";
        result += "\tdname=" + t.dname + "\n";
        result += "\thash=" + t.hash + "\n";
        result += "\tcategoryid=" + TOSTRING(t.categoryid) + "\n";
        result += "\tuniqueid=" + t.uniqueid + "\n";
        result += "\tguid=" + t.guid + "\n";
        result += "\tversion=" + TOSTRING(t.version) + "\n";
        result += "\tfilecachename=" + t.filecachename + "\n";
        //result += "\tnumauthors="+TOSTRING(t.authors.size())+"\n";

        if (t.authors.size() > 0)
        {
            for (int i = 0; i < (int)t.authors.size(); i++)
            {
                if (t.authors[i].type.empty())
                    t.authors[i].type = "unknown";
                if (t.authors[i].name.empty())
                    t.authors[i].name = "unknown";
                if (t.authors[i].email.empty())
                    t.authors[i].email = "unknown";
                result += "\tauthor=" + (t.authors[i].type) +
                    "," + TOSTRING(t.authors[i].id) +
                    "," + (t.authors[i].name) + "," + (t.authors[i].email) + "\n";
            }
        }

        // now add the truck details if existing
        if (t.description != "")
            result += "\tdescription=" + normalizeText(t.description) + "\n";
        if (t.tags != "")
            result += "\ttags=" + t.tags + "\n";
        if (t.fileformatversion != 0)
            result += "\tfileformatversion=" + TOSTRING(t.fileformatversion) + "\n";
        if (t.hasSubmeshs)
            result += "\thasSubmeshs=1\n";
        if (t.nodecount != 0)
            result += "\tnodecount=" + TOSTRING(t.nodecount) + "\n";
        if (t.beamcount != 0)
            result += "\tbeamcount=" + TOSTRING(t.beamcount) + "\n";
        if (t.shockcount != 0)
            result += "\tshockcount=" + TOSTRING(t.shockcount) + "\n";
        if (t.fixescount != 0)
            result += "\tfixescount=" + TOSTRING(t.fixescount) + "\n";
        if (t.hydroscount != 0)
            result += "\thydroscount=" + TOSTRING(t.hydroscount) + "\n";
        if (t.wheelcount != 0)
            result += "\twheelcount=" + TOSTRING(t.wheelcount) + "\n";
        if (t.propwheelcount != 0)
            result += "\tpropwheelcount=" + TOSTRING(t.propwheelcount) + "\n";
        if (t.commandscount != 0)
            result += "\tcommandscount=" + TOSTRING(t.commandscount) + "\n";
        if (t.flarescount != 0)
            result += "\tflarescount=" + TOSTRING(t.flarescount) + "\n";
        if (t.propscount != 0)
            result += "\tpropscount=" + TOSTRING(t.propscount) + "\n";
        if (t.wingscount != 0)
            result += "\twingscount=" + TOSTRING(t.wingscount) + "\n";
        if (t.turbopropscount != 0)
            result += "\tturbopropscount=" + TOSTRING(t.turbopropscount) + "\n";
        if (t.turbojetcount != 0)
            result += "\tturbojetcount=" + TOSTRING(t.turbojetcount) + "\n";
        if (t.rotatorscount != 0)
            result += "\trotatorscount=" + TOSTRING(t.rotatorscount) + "\n";
        if (t.exhaustscount != 0)
            result += "\texhaustscount=" + TOSTRING(t.exhaustscount) + "\n";
        if (t.flexbodiescount != 0)
            result += "\tflexbodiescount=" + TOSTRING(t.flexbodiescount) + "\n";
        if (t.materialflarebindingscount != 0)
            result += "\tmaterialflarebindingscount=" + TOSTRING(t.materialflarebindingscount) + "\n";
        if (t.soundsourcescount != 0)
            result += "\tsoundsourcescount=" + TOSTRING(t.soundsourcescount) + "\n";
        if (t.managedmaterialscount != 0)
            result += "\tmanagedmaterialscount=" + TOSTRING(t.managedmaterialscount) + "\n";
        if (t.truckmass > 1)
            result += "\ttruckmass=" + TOSTRING(t.truckmass) + "\n";
        if (t.loadmass > 1)
            result += "\tloadmass=" + TOSTRING(t.loadmass) + "\n";
        if (t.minrpm > 1)
            result += "\tminrpm=" + TOSTRING(t.minrpm) + "\n";
        if (t.maxrpm > 1)
            result += "\tmaxrpm=" + TOSTRING(t.maxrpm) + "\n";
        if (t.torque > 1)
            result += "\ttorque=" + TOSTRING(t.torque) + "\n";
        if (t.customtach)
            result += "\tcustomtach=1\n";
        if (t.custom_particles)
            result += "\tcustom_particles=1\n";
        if (t.forwardcommands)
            result += "\tforwardcommands=1\n";
        if (t.importcommands)
            result += "\timportcommands=1\n";
        if (t.rollon)
            result += "\trollon=1\n";
        if (t.rescuer)
            result += "\trescuer=1\n";
        if (t.driveable != 0)
            result += "\tdriveable=" + TOSTRING(t.driveable) + "\n";
        if (t.numgears != 0)
            result += "\tnumgears=" + TOSTRING(t.numgears) + "\n";
        if (t.enginetype != 0)
            result += "\tenginetype=" + TOSTRING(t.enginetype) + "\n";
        if (t.materials.size())
        {
            String matStr = "";
            for (std::set<Ogre::String>::iterator it = t.materials.begin(); it != t.materials.end(); it++)
            {
                matStr += *it + " ";
            }
            result += "\tmaterials=" + matStr + "\n";
        }

        if (t.sectionconfigs.size() > 0)
        {
            for (int i = 0; i < (int)t.sectionconfigs.size(); i++)
                result += "\tsectionconfig=" + t.sectionconfigs[i] + "\n";
        }
    }

    return result;
}

Ogre::String CacheSystem::normalizeText(Ogre::String text)
{
    String result = "";
    Ogre::StringVector str = Ogre::StringUtil::split(text, "\n");
    for (Ogre::StringVector::iterator it = str.begin(); it != str.end(); it++)
        result += *it + "$";
    return result;
}

Ogre::String CacheSystem::deNormalizeText(Ogre::String text)
{
    String result = "";
    Ogre::StringVector str = Ogre::StringUtil::split(text, "$");
    for (Ogre::StringVector::iterator it = str.begin(); it != str.end(); it++)
        result += *it + "\n";
    return result;
}



void CacheSystem::writeGeneratedCache()
{
    String path = getCacheConfigFilename(true);
    LOG("writing cache to file ("+path+")...");

    FILE* f = fopen(path.c_str(), "w");
    if (!f)
    {
        ErrorUtils::ShowError(_L("Fatal Error: Unable to write cache to disk"), _L("Unable to write file.\nPlease ensure the parent directories exists and that you have write access to this location:\n") + path);
        exit(1337);
    }
    fprintf(f, "shaone=%s\n", const_cast<char*>(currentSHA1.c_str()));
    fprintf(f, "modcount=%d\n", (int)entries.size());
    fprintf(f, "cacheformat=%s\n", CACHE_FILE_FORMAT);

    // mods
    std::vector<CacheEntry>::iterator it;
    int counter = 0;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        if (it->deleted)
            continue;
        fprintf(f, "%s", formatEntry(counter, *it).c_str());
        counter++;
    }

    // close
    fclose(f);
    LOG("...done!");
}



char* CacheSystem::replacesSpaces(char* str)
{
    char* ptr = str;
    while (*ptr != 0)
    {
        if (*ptr == ' ')
            *ptr = '_';
        ptr++;
    };
    return str;
}

char* CacheSystem::restoreSpaces(char* str)
{
    char* ptr = str;
    while (*ptr != 0)
    {
        if (*ptr == '_')
            *ptr = ' ';
        ptr++;
    };
    return str;
}

bool CacheSystem::stringHasUID(Ogre::String uidstr)
{
    size_t pos = uidstr.find("-");
    if (pos != String::npos && pos >= 3 && uidstr.substr(pos - 3, 3) == "UID")
        return true;
    return false;
}

Ogre::String CacheSystem::stripUIDfromString(Ogre::String uidstr)
{
    size_t pos = uidstr.find("-");
    if (pos != String::npos && pos >= 3 && uidstr.substr(pos - 3, 3) == "UID")
        return uidstr.substr(pos + 1, uidstr.length() - pos);
    return uidstr;
}

Ogre::String CacheSystem::getUIDfromString(Ogre::String uidstr)
{
    size_t pos = uidstr.find("-");
    if (pos != String::npos && pos >= 3 && uidstr.substr(pos - 3, 3) == "UID")
        return uidstr.substr(0, pos);
    return "";
}

void CacheSystem::addFile(Ogre::FileInfo f, String ext)
{
    String archiveType = "FileSystem";
    String archiveDirectory = "";
    if (f.archive)
    {
        archiveType = f.archive->getType();
        archiveDirectory = f.archive->getName();
    }

    addFile(f.filename, archiveType, archiveDirectory, ext);
}



int CacheSystem::addUniqueString(std::set<Ogre::String>& list, Ogre::String str)
{
    // ignore some render texture targets
    if (str == "mirror")
        return 0;
    if (str == "renderdash")
        return 0;

    str = stripUIDfromString(str);

    if (list.find(str) == list.end())
    {
        list.insert(str);
        return 1;
    }
    return 0;
}





Ogre::String CacheSystem::detectFilesMiniType(String filename)
{
    if (resourceExistsInAllGroups(filename + ".dds"))
        return "dds";

    if (resourceExistsInAllGroups(filename + ".png"))
        return "png";

    if (resourceExistsInAllGroups(filename + ".jpg"))
        return "jpg";

    return "none";
}





System::checkForNewFiles(Ogre::String ext)
{
    char fname[256];
    sprintf(fname, "*.%s", ext.c_str());

    StringVector sv = ResourceGroupManager::getSingleton().getResourceGroups();
    for (StringVector::iterator it = sv.begin(); it != sv.end(); ++it)
    {
        FileInfoListPtr files = ResourceGroupManager::getSingleton().findResourceFileInfo(*it, fname);
        for (FileInfoList::iterator iterFiles = files->begin(); iterFiles != files->end(); ++iterFiles)
        {
            String fn = iterFiles->filename.c_str();
            if (!isFileInEntries(fn))
            {
                if (iterFiles->archive->getType() == "Zip")
                LOG("- " + fn + " is new (in zip)");
                else
                LOG("- " + fn + " is new");
                newFiles++;
                addFile(*iterFiles, ext);
            }
        }
    }
}

String CacheSystem::filenamesSHA1()
{
    String filenames = "";

    // get all Files
    /*
    StringVector sv = ResourceGroupManager::getSingleton().getResourceGroups();
    StringVector::iterator it;
    for (it = sv.begin(); it!=sv.end(); it++)
    {
        StringVectorPtr files = ResourceGroupManager::getSingleton().listResourceNames(*it);
        for (StringVector::iterator i=files->begin(); i!=files->end(); i++)
        {
            // only use the important files :)
            for (std::vector<Ogre::String>::iterator sit=known_extensions.begin();sit!=known_extensions.end();sit++)
                if (i->find("."+*sit) != String::npos && i->find(".dds") == String::npos && i->find(".png") == String::npos)
                    filenames += "General/" + *i + "\n";
        }
    }
    */

    // and we use all folders and files for the hash
    String restype[3] = {"Packs", "TerrainFolders", "VehicleFolders"};
    for (int i = 0; i < 3; i++)
    {
        for (int b = 0; b < 2; b++)
        {
            FileInfoListPtr list = ResourceGroupManager::getSingleton().listResourceFileInfo(restype[i], (b == 1));
            for (FileInfoList::iterator iterFiles = list->begin(); iterFiles != list->end(); iterFiles++)
            {
                String name = restype[i] + "/";
                if (iterFiles->archive)
                    name += iterFiles->archive->getName() + "/";

                if (b == 0)
                {
                    // special file handling, only add important files!
                    bool vipfile = false;
                    for (std::vector<Ogre::String>::iterator sit = known_extensions.begin(); sit != known_extensions.end(); sit++)
                    {
                        if ((iterFiles->filename.find("." + *sit) != String::npos && iterFiles->filename.find(".dds") == String::npos && iterFiles->filename.find(".png") == String::npos && iterFiles->filename.find(".jpg") == String::npos)
                            || (iterFiles->filename.find(".zip") != String::npos))
                        {
                            vipfile = true;
                            break;
                        }
                    }
                    if (!vipfile)
                        continue;
                }
                name += iterFiles->filename;
                filenames += name + "\n";
            }
        }
    }

    char result[256] = {};

    RoR::CSHA1 sha1;
    char* data = const_cast<char*>(filenames.c_str());
    sha1.UpdateHash((uint8_t *)data, (uint32_t)strlen(data));
    sha1.Final();
    sha1.ReportHash(result, RoR::CSHA1::REPORT_HEX_SHORT);
    return result;
}








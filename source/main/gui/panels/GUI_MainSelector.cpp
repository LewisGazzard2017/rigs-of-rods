/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013+     Petr Ohlidal & contributors

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

#include "GUI_MainSelector.h"

#include "RoRPrerequisites.h"
#include "Utils.h"
#include "Language.h"
#include "GUIManager.h"
#include "GUI_LoadingWindow.h"
#include "InputEngine.h"
#include "Application.h"
#include "CacheSystem.h"
#include "ContentManager.h"
#include "SkinManager.h"
#include "RoRFrameListener.h"
#include "MainThread.h"
#include "ModCache.h"

#include <MyGUI.h>

using namespace RoR;
using namespace GUI;

#define CLASS        MainSelector
#define MAIN_WIDGET  ((MyGUI::Window*)mMainWidget)

CLASS::CLASS() :
    m_keys_bound(false)
    , m_selected_skin(nullptr)
    , m_selected_entry(nullptr)
    , m_selection_done(true)
{
    MAIN_WIDGET->setVisible(false);
    m_skin_manager = RoR::App::GetContentManager()->GetSkinManager();

    MyGUI::WindowPtr win = dynamic_cast<MyGUI::WindowPtr>(mMainWidget);
    win->eventWindowButtonPressed += MyGUI::newDelegate(this, &CLASS::NotifyWindowButtonPressed); //The "X" button thing
    win->eventWindowChangeCoord += MyGUI::newDelegate(this, &CLASS::NotifyWindowChangeCoord);

    MyGUI::IntSize windowSize = MAIN_WIDGET->getSize();
    MyGUI::IntSize parentSize = MAIN_WIDGET->getParentSize();

    m_Type->eventComboChangePosition += MyGUI::newDelegate(this, &CLASS::EventComboChangePositionTypeComboBox);

    m_Model->eventListSelectAccept += MyGUI::newDelegate(this, &CLASS::EventListChangePositionModelListAccept);
    m_Model->eventListChangePosition += MyGUI::newDelegate(this, &CLASS::EventListChangePositionModelList);
    m_Config->eventComboAccept += MyGUI::newDelegate(this, &CLASS::EventComboAcceptConfigComboBox);
    m_Ok->eventMouseButtonClick += MyGUI::newDelegate(this, &CLASS::EventMouseButtonClickOkButton);
    m_Cancel->eventMouseButtonClick += MyGUI::newDelegate(this, &CLASS::EventMouseButtonClickCancelButton);

    // search stuff
    m_SearchLine->eventEditTextChange += MyGUI::newDelegate(this, &CLASS::EventSearchTextChange);
    m_SearchLine->eventMouseSetFocus += MyGUI::newDelegate(this, &CLASS::EventSearchTextGotFocus);
    m_SearchLine->eventKeySetFocus += MyGUI::newDelegate(this, &CLASS::EventSearchTextGotFocus);

    MAIN_WIDGET->setPosition((parentSize.width - windowSize.width) / 2, (parentSize.height - windowSize.height) / 2);

    //From old file
    MAIN_WIDGET->setCaption(_L("Loader"));

    m_SearchLine->setCaption("");
    m_Ok->setCaption(_L("OK"));
    m_Cancel->setCaption(_L("Cancel"));

    // setup controls
    m_Config->addItem("Default", Ogre::String("Default"));
    m_Config->setIndexSelected(0);

    MAIN_WIDGET->setRealPosition(0.1, 0.1);
    MAIN_WIDGET->setRealSize(0.8, 0.8);
}

CLASS::~CLASS()
{
}

void CLASS::Reset()
{
    m_selected_skin = nullptr;
    m_selected_entry = nullptr;
    m_selection_done = true;
}

void CLASS::BindKeys(bool bind)
{
    if (bind && !m_keys_bound)
    {
        MAIN_WIDGET->eventKeyButtonPressed += MyGUI::newDelegate(this, &CLASS::EventKeyButtonPressed_Main);
        m_Type->eventKeyButtonPressed += MyGUI::newDelegate(this, &CLASS::EventKeyButtonPressed_Main);
        m_SearchLine->eventKeyButtonPressed += MyGUI::newDelegate(this, &CLASS::EventKeyButtonPressed_Main);
        m_keys_bound = true;
    }
    else if (!bind && m_keys_bound)
    {
        MAIN_WIDGET->eventKeyButtonPressed -= MyGUI::newDelegate(this, &CLASS::EventKeyButtonPressed_Main);
        m_Type->eventKeyButtonPressed -= MyGUI::newDelegate(this, &CLASS::EventKeyButtonPressed_Main);
        m_SearchLine->eventKeyButtonPressed -= MyGUI::newDelegate(this, &CLASS::EventKeyButtonPressed_Main);
        m_keys_bound = false;
    }
}

void CLASS::NotifyWindowChangeCoord(MyGUI::Window* _sender)
{
    if (m_Preview->isVisible())
        ResizePreviewImage();
}

void CLASS::EventKeyButtonPressed_Main(MyGUI::WidgetPtr _sender, MyGUI::KeyCode _key, MyGUI::Char _char)
{
    if (!mMainWidget->getVisible())
        return;
    int cid = (int)m_Type->getIndexSelected();
    int iid = (int)m_Model->getIndexSelected();

    bool searching = MyGUI::InputManager::getInstance().getKeyFocusWidget() == m_SearchLine;

    if (_key == MyGUI::KeyCode::Slash)
    {
        MyGUI::InputManager::getInstance().setKeyFocusWidget(m_SearchLine);
        m_SearchLine->setCaption("");
        searching = true;
    }
    else if (_key == MyGUI::KeyCode::Tab)
    {
        if (searching)
        {
            MyGUI::InputManager::getInstance().setKeyFocusWidget(mMainWidget);
            m_SearchLine->setCaption(_L("Search ..."));
        }
        else
        {
            MyGUI::InputManager::getInstance().setKeyFocusWidget(m_SearchLine);
            m_SearchLine->setCaption("");
        }
        searching = !searching;
    }

    // category
    if (!searching && (_key == MyGUI::KeyCode::ArrowLeft || _key == MyGUI::KeyCode::ArrowRight))
    {
        int newitem = cid;

        if (_key == MyGUI::KeyCode::ArrowLeft)
        {
            newitem--;
            if (cid == 0)
            {
                newitem = (int)m_Type->getItemCount() - 1;
            }
        }
        else
        {
            newitem++;
            if (cid == (int)m_Type->getItemCount() - 1)
            {
                newitem = 0;
            }
        }

        try
        {
            m_Type->setIndexSelected(newitem);
            m_Type->beginToItemSelected();
        }
        catch (...)
        {
            return;
        }
        EventComboChangePositionTypeComboBox(m_Type, newitem);
    }
    else if (_key == MyGUI::KeyCode::ArrowUp || _key == MyGUI::KeyCode::ArrowDown)
    {
        int newitem = iid;

        if (_key == MyGUI::KeyCode::ArrowUp)
            newitem--;
        else
            newitem++;


        if (iid == 0 && _key == MyGUI::KeyCode::ArrowUp)
        {
            newitem = (int)m_Model->getItemCount() - 1;
        }
        else if (iid == (int)m_Model->getItemCount() - 1 && _key == MyGUI::KeyCode::ArrowDown)
        {
            newitem = 0;
        }

        try
        {
            m_Model->setIndexSelected(newitem);
            m_Model->beginToItemSelected();
        }
        catch (...)
        {
            return;
        }

        EventListChangePositionModelList(m_Model, newitem);

        // fix cursor position
        if (searching)
        {
            m_SearchLine->setTextCursor(m_SearchLine->getTextLength());
        }
    }
    else if (_key == MyGUI::KeyCode::Return)
    {
        if (m_loader_type == LT_SKIN || (m_loader_type != LT_SKIN && m_selected_entry))
        {
            OnSelectionDone();
        }
    }
}

void CLASS::Cancel()
{
    m_selected_entry = nullptr;
    m_selection_done = true;
    Hide();

    if (App::GetActiveAppState() == App::APP_STATE_MAIN_MENU)
    {
        RoR::App::GetMainThreadLogic()->LeaveMultiplayerServer();
        App::GetGuiManager()->SetVisible_GameMainMenu(true);
    }
}

void CLASS::EventMouseButtonClickOkButton(MyGUI::WidgetPtr _sender)
{
    OnSelectionDone();
}

void CLASS::EventMouseButtonClickCancelButton(MyGUI::WidgetPtr _sender)
{
    Cancel();
}

void CLASS::EventComboChangePositionTypeComboBox(MyGUI::ComboBoxPtr _sender, size_t _index)
{
    if (!MAIN_WIDGET->getVisible())
        return;
    try
    {
        int categoryID = *m_Type->getItemDataAt<int>(_index);
        m_SearchLine->setCaption(_L("Search ..."));
        OnCategorySelected(categoryID);
    }
    catch (...)
    {
    }
}

void CLASS::EventListChangePositionModelListAccept(MyGUI::ListPtr _sender, size_t _index)
{
    EventListChangePositionModelList(_sender, _index);
    OnSelectionDone();
}

void CLASS::EventListChangePositionModelList(MyGUI::ListPtr _sender, size_t _index)
{
    if (!MAIN_WIDGET->getVisible())
        return;

    if (_index < 0 || _index >= m_Model->getItemCount())
        return;

    try
    {
        int entryID = *m_Model->getItemDataAt<int>(_index);
        OnEntrySelected(entryID);
    }
    catch (...)
    {
    }
}

void CLASS::EventComboAcceptConfigComboBox(MyGUI::ComboBoxPtr _sender, size_t _index)
{
    if (!MAIN_WIDGET->getVisible())
        return;
    try
    {
        m_vehicle_configs.clear();
        Ogre::String config = *m_Config->getItemDataAt<Ogre::String>(_index);
        m_vehicle_configs.push_back(config);
    }
    catch (...)
    {
    }
}

template <typename T1, typename T2>
struct sort_cats
{
    bool operator ()(std::pair<int, Category_Entry> const& a, std::pair<int, Category_Entry> const& b) const
    {
        if (a.second.number == CacheSystem::CID_All)
            return true;
        if (b.second.number == CacheSystem::CID_All)
            return false;
        if (a.second.number == CacheSystem::CID_Fresh)
            return true;
        if (b.second.number == CacheSystem::CID_Fresh)
            return false;
        return a.second.title < b.second.title;
    }
};

template <typename T1>
struct sort_entries
{
    bool operator ()(CacheEntry const& a, CacheEntry const& b) const
    {
        Ogre::String first = a.dname;
        Ogre::String second = b.dname;
        Ogre::StringUtil::toLowerCase(first);
        Ogre::StringUtil::toLowerCase(second);
        return first < second;
    }
};

struct sort_search_results
{
    bool operator ()(std::pair<ModCache::Entry*, size_t> const& a, std::pair<ModCache::Entry*, size_t> const& b) const
    {
        return a.second < b.second;
    }
};

typedef std::map<ModCache::Entry::Category, int> CategoryUsageMap;

inline void AddCategoryUsage(CategoryUsageMap& map, ModCache::Entry* entry, size_t cur_timestamp)
{
    ++map[entry->category];
    ++map[ModCache::Entry::CATEGORY_SPECIAL_ALL];

    if ((cur_timestamp - entry->added_timestamp) < CACHE_FILE_FRESHNESS)
    {
        ++map[ModCache::Entry::CATEGORY_SPECIAL_FRESH];
    }
}

void CLASS::UpdateGuiData()
{
    if (! ModCache::IsInitFinished())
    {
        return;
    }

    m_Type->removeAllItems();
    m_Model->removeAllItems();

    if (m_loader_type == LT_SKIN)
    {
        // skin specific stuff
        m_Type->setEnabled(false);
        m_Type->setCaption(_L("Skins"));
        m_Cancel->setEnabled(false);
        m_Config->setVisible(false);

        m_Model->addItem(_L("Default Skin"), 0);
        {
            int i = 1;
            for (std::vector<Skin *>::iterator it = m_current_skins.begin(); it != m_current_skins.end(); it++ , i++)
            {
                m_Model->addItem((*it)->getName(), i);
            }
        }
        m_Model->setIndexSelected(0);
        OnEntrySelected(0);
        return;
    }
    else
    {
        m_Type->setEnabled(true);
        m_Cancel->setEnabled(true);
    }

    CategoryUsageMap category_usage;
    size_t cur_timestamp = (size_t) getTimeStamp();

    // Update terrains
    m_terrain_entries.clear();
    for (ModCache::TerrainEntry& terrn : *ModCache::GetTerrains())
    {
        AddCategoryUsage(category_usage, &terrn, cur_timestamp);

        m_terrain_entries.push_back(&terrn);
    }

    // Update softbodies
    const bool take_truck    = (m_loader_type == LT_AllBeam || m_loader_type == LT_Vehicle || m_loader_type == LT_Truck || m_loader_type == LT_Network || m_loader_type == LT_NetworkWithBoat);
    const bool take_car      = (m_loader_type == LT_AllBeam || m_loader_type == LT_Vehicle || m_loader_type == LT_Car || m_loader_type == LT_Network || m_loader_type == LT_NetworkWithBoat);
    const bool take_boat     = (m_loader_type == LT_AllBeam || m_loader_type == LT_Boat || m_loader_type == LT_NetworkWithBoat);
    const bool take_airplane = (m_loader_type == LT_AllBeam || m_loader_type == LT_Airplane || m_loader_type == LT_Network || m_loader_type == LT_NetworkWithBoat);
    const bool take_trailer  = (m_loader_type == LT_AllBeam || m_loader_type == LT_Trailer || m_loader_type == LT_Extension);
    const bool take_train    = (m_loader_type == LT_AllBeam || m_loader_type == LT_Train);
    const bool take_load     = (m_loader_type == LT_AllBeam || m_loader_type == LT_Load || m_loader_type == LT_Extension);

    for (ModCache::SoftbodyEntry& softbody: *ModCache::GetSoftbodies())
    {
        if ((softbody.file_ext == "truck"     && !take_truck   ) ||
            (softbody.file_ext == "car"       && !take_car     ) ||
            (softbody.file_ext == "boat"      && !take_boat    ) ||
            (softbody.file_ext == "airplane"  && !take_airplane) ||
            (softbody.file_ext == "trailer"   && !take_trailer ) ||
            (softbody.file_ext == "train"     && !take_train   ) ||
            (softbody.file_ext == "load"      && !take_load    ))
        {
            continue; // Skip
        }

        AddCategoryUsage(category_usage, &softbody, cur_timestamp);

        m_softbody_entries.push_back(&softbody);
    }

    int tally_categories = 0, current_category = 0;
    ModCache::CategoryInfoMap& cats = ModCache::GetCategories();

    std::vector<std::pair<int, ModCache::CategoryInfo> > sorted_cats(cats.begin(), cats.end());
    std::sort(sorted_cats.begin(), sorted_cats.end(), sort_cats<int, ModCache::CategoryInfo>());

    for (std::vector<std::pair<int, ModCache::CategoryInfo>>::iterator itc = sorted_cats.begin(); itc != sorted_cats.end(); itc++)
    {
        if (category_usage[itc->second.id] > 0)
            tally_categories++;
    }
    
    for (std::vector<std::pair<int, ModCache::CategoryInfo>>::iterator itc = sorted_cats.begin(); itc != sorted_cats.end(); itc++)
    {
        int num_elements = category_usage[itc->second.id];
        if (num_elements > 0)
        {
            Ogre::UTFString title = _L("unknown");
            if (!itc->second.title.empty())
            {
                title = _L(itc->second.title.c_str());
            }
            Ogre::UTFString txt = U("[") + TOUTFSTRING(++current_category) + U("/") + TOUTFSTRING(tally_categories) + U("] (") + TOUTFSTRING(num_elements) + U(") ") + title;
            m_Type->addItem(convertToMyGUIString(txt), itc->second.id);
        }
    }
    if (tally_categories > 0)
    {
        try
        {
            m_Type->setIndexSelected(0);
            m_Type->beginToItemSelected();
        }
        catch (...)
        {
            return;
        }
        OnCategorySelected(*m_Type->getItemDataAt<int>(0));
    }
}

size_t CLASS::SearchCompare(Ogre::String searchString, ModCache::Entry* ce)
{
    if (searchString.find(":") == Ogre::String::npos)
    {
        // normal search

        // the name
        Ogre::String dname_lower = ce->name;
        Ogre::StringUtil::toLowerCase(dname_lower);
        if (dname_lower.find(searchString) != Ogre::String::npos)
            return dname_lower.find(searchString);

        // the filename
        Ogre::String fname_lower = ce->filename;
        Ogre::StringUtil::toLowerCase(fname_lower);
        if (fname_lower.find(searchString) != Ogre::String::npos)
            return 100 + fname_lower.find(searchString);

        // the description
        Ogre::String desc = ce->description;
        Ogre::StringUtil::toLowerCase(desc);
        if (desc.find(searchString) != Ogre::String::npos)
            return 200 + desc.find(searchString);

        // the authors
        if (!ce->authors.empty())
        {
            std::vector<ModCache::AuthorInfo>::const_iterator it;
            for (it = ce->authors.begin(); it != ce->authors.end(); it++)
            {
                // author name
                Ogre::String aname = it->name;
                Ogre::StringUtil::toLowerCase(aname);
                if (aname.find(searchString) != Ogre::String::npos)
                    return 300 + aname.find(searchString);

                // author email
                Ogre::String aemail = it->email;
                Ogre::StringUtil::toLowerCase(aemail);
                if (aemail.find(searchString) != Ogre::String::npos)
                    return 400 + aemail.find(searchString);
            }
        }
        return Ogre::String::npos;
    }
    else
    {
        Ogre::StringVector v = Ogre::StringUtil::split(searchString, ":");
        if (v.size() < 2)
            return Ogre::String::npos; //invalid syntax

        else if (v[0] == "author")
        {
            // the authors
            if (!ce->authors.empty())
            {
                std::vector<ModCache::AuthorInfo>::const_iterator it;
                for (it = ce->authors.begin(); it != ce->authors.end(); it++)
                {
                    // author name
                    Ogre::String aname = it->name;
                    Ogre::StringUtil::toLowerCase(aname);
                    if (aname.find(v[1]) != Ogre::String::npos)
                        return aname.find(v[1]);

                    // author email
                    Ogre::String aemail = it->email;
                    Ogre::StringUtil::toLowerCase(aemail);
                    if (aemail.find(v[1]) != Ogre::String::npos)
                        return aemail.find(v[1]);
                }
            }
            return Ogre::String::npos;
        }
        else if (v[0] == "file")
        {
            Ogre::String fn = ce->filename;
            Ogre::StringUtil::toLowerCase(fn);
            return fn.find(v[1]);
        }
    }
    return Ogre::String::npos;
}

void CLASS::OnCategorySelected(int categoryID)
{
    if (m_loader_type == LT_SKIN)
        return;

    Ogre::String search_cmd = m_SearchLine->getCaption();
    Ogre::StringUtil::toLowerCase(search_cmd);

    int counter = 0;
    int ts = getTimeStamp();

    m_Model->removeAllItems();

    if (categoryID == CacheSystem::CID_SearchResults)
    {
        std::vector<std::pair<ModCache::Entry*, size_t> > search_results;

        if (m_loader_type == LT_Terrain)
        {
            for (ModCache::TerrainEntry* terrn : m_terrain_entries)
            {
                size_t score = SearchCompare(search_cmd, terrn);
                if (score != Ogre::String::npos)
                    search_results.push_back(std::make_pair(terrn, score));
            }
        }
        else
        {
            for (ModCache::SoftbodyEntry* sbody : m_softbody_entries)
            {
                size_t score = SearchCompare(search_cmd, sbody);
                if (score != Ogre::String::npos)
                    search_results.push_back(std::make_pair(sbody, score));
            }
        }

        std::stable_sort(search_results.begin(), search_results.end(), sort_search_results());

        for (auto it = search_results.begin(); it != search_results.end(); it++)
        {
            counter++;
            Ogre::String txt = TOSTRING(counter) + ". " + it->first->name;
            try
            {
                m_Model->addItem(txt, it->first->category);
            }
            catch (...)
            {
                m_Model->addItem("ENCODING ERROR", it->first->category);
            }
        }
    }
    else if (m_loader_type == LT_Terrain)
    {
        size_t terrn_index = 0;
        for (ModCache::TerrainEntry* entry : m_terrain_entries)
        {
            ModCache::Entry::Category c = entry->category;
            if (c == categoryID || c == ModCache::Entry::CATEGORY_SPECIAL_ALL
                || (c == ModCache::Entry::CATEGORY_SPECIAL_FRESH && ts - entry->added_timestamp < CACHE_FILE_FRESHNESS))
            {
                counter++;
                Ogre::String txt = TOSTRING(counter) + ". " + Utils::SanitizeUtf8String(entry->name);
                m_Model->addItem(txt, terrn_index);
            }
            ++terrn_index;
        }
    }
    else
    {
        size_t softbody_index = 0;
        for (ModCache::SoftbodyEntry* entry : m_softbody_entries)
        {
            ModCache::Entry::Category c = entry->category;
            if (c == categoryID || c == ModCache::Entry::CATEGORY_SPECIAL_ALL
                || (c == ModCache::Entry::CATEGORY_SPECIAL_FRESH && ts - entry->added_timestamp < CACHE_FILE_FRESHNESS))
            {
                counter++;
                Ogre::String txt = TOSTRING(counter) + ". " + Utils::SanitizeUtf8String(entry->name);
                m_Model->addItem(txt, softbody_index);
            }
            ++softbody_index;
        }
    }

    if (counter > 0)
    {
        try
        {
            m_Model->setIndexSelected(0);
            m_Model->beginToItemSelected();
        }
        catch (...)
        {
            return;
        }
        OnEntrySelected(*m_Model->getItemDataAt<int>(0));
    }
}

void CLASS::OnEntrySelected(int entryID)
{
    if (m_loader_type == LT_SKIN)
    {
        // special skin handling
        if (entryID == 0)
        {
            // default, default infos
            m_selected_skin = 0;
            this->UpdateControls(m_selected_entry);
            return;
        }
        entryID -= 1; // remove default skin :)
        Skin* skin = m_current_skins[entryID];

        // we assume its already loaded
        // set selected skin as current
        m_selected_skin = skin;

        SetPreviewImage(m_current_skins[entryID]->thumbnail);

        m_EntryName->setCaption(skin->name);

        Ogre::UTFString descriptiontxt = skin->description + Ogre::String("\n");
        descriptiontxt = descriptiontxt + _L("Author(s): ") + skin->authorName + Ogre::String("\n");
        descriptiontxt = descriptiontxt + _L("Description: ") + skin->description + Ogre::String("\n");

        try
        {
            m_EntryDescription->setCaption(Ogre::String(descriptiontxt));
        }
        catch (...)
        {
            m_EntryDescription->setCaption("ENCODING ERROR");
        }
        return;
    }
    
    if (m_loader_type == LT_Terrain)
    {
        m_selected_entry = m_terrain_entries.at(entryID);
        if (m_selected_entry != nullptr)
        {
            this->UpdateControls(m_selected_entry);
        }
        return;
    }
    
    // Default = assume softbody
    m_selected_entry = m_terrain_entries.at(entryID);
    if (m_selected_entry != nullptr)
    {
        this->UpdateControls(m_selected_entry);
    }
    
}

void CLASS::OnSelectionDone()
{
    if (!m_selected_entry || m_selection_done)
        return;

    m_selection_done = true;

    if (m_loader_type != LT_SKIN)
    {
        m_current_skins.clear();
        m_skin_manager->GetUsableSkins(m_selected_entry->guid, this->m_current_skins);
        if (!m_current_skins.empty())
        {
            Show(LT_SKIN);
        }
        else
        {
            m_selected_skin = 0;
            this->Hide(false); // Hide without fade-out effect
        }
    }
    else if ((m_loader_type == LT_Terrain) && (App::GetActiveMpState() == App::MP_STATE_CONNECTED))
    {
        App::SetPendingAppState(App::APP_STATE_SIMULATION);
        this->Hide(false);
    }
    else
    {
        // we show the skin loader, set final skin and exit!
        // m_selected_skin should be set already!
        Hide();
    }
}

void CLASS::UpdateControls(ModCache::Entry* entry)
{
    Ogre::String outBasename = entry->filename;

    SetPreviewImage(outBasename);

    ModCache::SoftbodyEntry* sb_entry = nullptr;
    bool has_modules = false;
    if (entry->GetType() == ModCache::Entry::TYPE_SOFTBODY)
    {
        sb_entry = static_cast<ModCache::SoftbodyEntry*>(entry);
        if (!sb_entry->modules.empty())
        {
            has_modules = true;
            m_Config->setVisible(true);
            m_Config->removeAllItems();
            for (std::string module_name: sb_entry->modules)
            {
                module_name = Utils::SanitizeUtf8String(module_name);
                m_Config->addItem(module_name, module_name);
            }
            m_Config->setIndexSelected(0);

            m_vehicle_configs.clear();
            Ogre::String configstr = *m_Config->getItemDataAt<Ogre::String>(0);
            m_vehicle_configs.push_back(configstr);
        }
    }

    if (!has_modules)
    {
        m_Config->setVisible(false);
    }

    Ogre::UTFString authors = "";
    std::set<Ogre::String> author_names;
    for (auto it = entry->authors.begin(); it != entry->authors.end(); it++)
    {
        if (!it->name.empty())
        {
            Ogre::String name = it->name;
            Ogre::StringUtil::trim(name);
            author_names.insert(name);
        }
    }
    for (std::set<Ogre::String>::iterator it = author_names.begin(); it != author_names.end(); it++)
    {
        Ogre::UTFString name = ANSI_TO_UTF(*it);
        authors.append(U(" ") + name);
    }
    if (authors.length() == 0)
    {
        authors = _L("no author information available");
    }

    m_EntryName->setCaption(Utils::SanitizeUtf8String(entry->name));

    Ogre::UTFString c = U("#FF7D02"); // colour key shortcut
    Ogre::UTFString nc = U("#FFFFFF"); // colour key shortcut

    Ogre::UTFString newline = U("\n");

    Ogre::UTFString descriptiontxt = U("#66FF33") + ANSI_TO_UTF(entry->description) + nc + newline;

    descriptiontxt = descriptiontxt + _L("Author(s): ") + c + authors + nc + newline;

    if (!entry->guid.empty() && entry->guid != "no-guid")
        descriptiontxt = descriptiontxt + _L("GUID: ") + c + entry->guid + nc + newline;

    if (entry->added_timestamp > 0)
    {
        char tmp[255] = "";
        time_t epch = entry->added_timestamp;
        sprintf(tmp, "%s", asctime(gmtime(&epch)));
        descriptiontxt = descriptiontxt + _L("Date and Time installed: ") + c + Ogre::String(tmp) + nc + newline;
    }

    Ogre::UTFString driveableStr[5] = {_L("Non-Driveable"), _L("Truck"), _L("Airplane"), _L("Boat"), _L("Machine")};
    if (sb_entry != nullptr)
    {
        descriptiontxt = descriptiontxt + _L("Vehicle Type: ") + c + driveableStr[sb_entry->driveable] + nc + newline;
    }

    if (has_modules)
    {
        descriptiontxt = descriptiontxt + U("\n\n#e10000") + _L("Please select a configuration below!") + nc + U("\n\n");
    }

    trimUTFString(descriptiontxt);

    m_EntryDescription->setCaption(convertToMyGUIString(descriptiontxt));
}

void CLASS::SetPreviewImage(Ogre::String texture)
{
    if (texture == "" || texture == "none")
    {
        m_Preview->setVisible(false);
        return;
    }

    m_preview_image_texture = texture;

    try
    {
        ResizePreviewImage();
        m_Preview->setImageTexture(texture);
        m_Preview->setVisible(true);
    }
    catch (...)
    {
        Ogre::LogManager::getSingleton().stream() << "[RoR|SelectorGUI] Failed to load preview image: " << m_preview_image_texture;
        m_Preview->setVisible(false);
    }
}

void CLASS::ResizePreviewImage()
{
    MyGUI::IntSize imgSize(0, 0);
    Ogre::TexturePtr t = Ogre::TextureManager::getSingleton().load(m_preview_image_texture, Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
    if (!t.isNull())
    {
        imgSize.width = (int)t->getWidth() * 10;
        imgSize.height = (int)t->getHeight() * 10;
    }

    if (imgSize.width != 0 && imgSize.height != 0)
    {
        MyGUI::IntSize maxSize = m_PreviewBox->getSize();

        float imgRatio = imgSize.width / (float)imgSize.height;
        float maxRatio = maxSize.width / (float)maxSize.height;

        MyGUI::IntSize newSize(0, 0);
        MyGUI::IntPoint newPosition(0, 0);

        // scale with aspect ratio
        if (imgRatio > maxRatio)
        {
            newSize.width = maxSize.width;
            newSize.height = maxSize.width / imgRatio;
            newPosition.left = 0;
            newPosition.top = maxSize.height - newSize.height;
        }
        else
        {
            newSize.width = maxSize.height * imgRatio;
            newSize.height = maxSize.height;
            newPosition.left = maxSize.width - newSize.width;
            newPosition.top = 0;
        }

        m_Preview->setSize(newSize);
        m_Preview->setPosition(newPosition);
    }
}

bool CLASS::IsFinishedSelecting()
{
    return m_selection_done;
}

void CLASS::Show(LoaderType type)
{
    if (!m_selection_done)
        return;
    m_selection_done = false;

    m_selected_skin = 0;
    m_SearchLine->setCaption("");
    RoR::App::GetInputEngine()->resetKeys();
    App::GetGuiManager()->SetVisible_LoadingWindow(false);
    m_vehicle_configs.clear();
    //MyGUI::InputManager::getInstance().setKeyFocusWidget(mMainWidget);
    MyGUI::InputManager::getInstance().setKeyFocusWidget(m_SearchLine);
    mMainWidget->setEnabledSilent(true);

    MAIN_WIDGET->setVisibleSmooth(true);

    if (type != LT_SKIN)
        m_selected_entry = nullptr; // when in skin, we still need the info

    m_loader_type = type;
    UpdateGuiData();

    BindKeys();

    if (type == LT_Terrain && (RoR::App::GetActiveMpState() == RoR::App::MP_STATE_CONNECTED))
    {
        m_Cancel->setCaption(_L("Cancel (disconnect)"));
    }
    else
    {
        m_Cancel->setCaption(_L("Cancel"));
    }
}

void CLASS::Hide(bool smooth)
{
    m_selection_done = true;
    RoR::App::GetGuiManager()->UnfocusGui();
    if (smooth)
    {
        MAIN_WIDGET->setVisibleSmooth(false);
    }
    else
    {
        MAIN_WIDGET->setVisible(false);
    }
    MAIN_WIDGET->setEnabledSilent(false);
    BindKeys(false);
}

void CLASS::EventSearchTextChange(MyGUI::EditBox* _sender)
{
    if (!MAIN_WIDGET->getVisible())
        return;
    OnCategorySelected(CacheSystem::CID_SearchResults);
    if (m_SearchLine->getTextLength() > 0)
    {
        m_Type->setCaption(_L("Search Results"));
    }
}

void CLASS::EventSearchTextGotFocus(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr oldWidget)
{
    if (!MAIN_WIDGET->getVisible())
        return;

    if (m_SearchLine->getCaption() == _L("Search ..."))
    {
        m_SearchLine->setCaption("");
    }
}

bool CLASS::IsVisible()
{
    return MAIN_WIDGET->isVisible();
}

void CLASS::NotifyWindowButtonPressed(MyGUI::WidgetPtr _sender, const std::string& _name)
{
    if (_name == "close")
    {
        Cancel();
    }
}

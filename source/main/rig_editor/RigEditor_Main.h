/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2015 Petr Ohlidal

	For more information, see http://www.rigsofrods.com/

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
	@file   RigEditor_Main.h
	@date   06/2014
	@author Petr Ohlidal
*/

#pragma once

#include "ConfigFile.h"
#include "GUI_OpenSaveFileDialog.h"
#include "RigDef_Prerequisites.h"
#include "RigEditor_ForwardDeclarations.h"
#include "RigEditor_IMain.h"
#include "RigEditor_InputHandler.h"
#include "RoRPrerequisites.h"

#include <string>

namespace RoR
{

namespace RigEditor
{

// Forward decl.
class CameraWrapper;

class Main: public IMain
{
public:

	Main(Config* config);
	~Main();

	void EnterMainLoop();

	// Getters...

	inline InputHandler&         GetInputHandler()       { return m_input_handler; }
	inline Ogre::SceneManager*   GetOgreSceneManager()   { return m_scene_manager; }
	inline Config*               GetConfig()             { return m_config; }
	inline CameraWrapper*        GetCameraWrapper()      { return m_camera_wrapper; }

	// Python interface...

    void PY_OnEnter_SetupInput();
    void PY_OnEnter_SetupCameraAndViewport();

private:
	Config*              m_config;
	Ogre::SceneManager*  m_scene_manager;
	Ogre::Viewport*      m_viewport;
	Ogre::Camera*        m_camera;
	InputHandler         m_input_handler;
	CameraWrapper*       m_camera_wrapper;


	/*
	// =============================================================================
	// TO BE CLEANED
	// =============================================================================


	// Rig updaters
	virtual void CommandRigSelectedNodesUpdateAttributes     (const RigAggregateNodesData*      data);
	virtual void CommandRigSelectedPlainBeamsUpdateAttributes(const RigAggregatePlainBeamsData* data);
	virtual void CommandRigSelectedShocksUpdateAttributes    (const RigAggregateShocksData*     data);
	virtual void CommandRigSelectedShocks2UpdateAttributes   (const RigAggregateShocks2Data*    data);
	virtual void CommandRigSelectedHydrosUpdateAttributes    (const RigAggregateHydrosData*     data);
	virtual void CommandRigSelectedCommands2UpdateAttributes (const RigAggregateCommands2Data*  data);

	// Wheel list
	virtual void CommandScheduleSetWheelSelected (LandVehicleWheel* wheel_ptr, int wheel_index, bool state_selected);
	virtual void CommandSetWheelHovered          (LandVehicleWheel* wheel_ptr, int wheel_index, bool state_hovered);
	virtual void CommandScheduleSetAllWheelsSelected (bool state_selected);
	virtual void CommandSetAllWheelsHovered          (bool state_selected);

    // Flare list
    virtual void CommandShowFlaresList();
	virtual void CommandScheduleSetFlareSelected (Flare* flare_ptr, int flare_index, bool state_selected);
	virtual void CommandSetFlareHovered          (Flare* flare_ptr, int flare_index, bool state_hovered);
	virtual void CommandScheduleSetAllFlaresSelected (bool state_selected);
	virtual void CommandSetAllFlaresHovered          (bool state_selected);

    // Interface getters
    virtual RigEditor::Node*      GetCurrentRigLastSelectedNode();

    // Interface factories
    virtual PointListDynamicMesh*    CreateInstanceOfPointListDynamicMesh(float point_size, size_t estimate_point_count);
    virtual LineListDynamicMesh*     CreateInstanceOfLineListDynamicMesh(size_t estimate_line_count);
    
    // --------------------------------------------
    //                 State flags
    // --------------------------------------------
    // --- Wheels ---
    BITMASK_PROPERTY(m_state_flags,  1,  IS_SELECT_WHEEL_SCHEDULED,         IsSelectWheelScheduled,         SetIsSelectWheelScheduled);
    BITMASK_PROPERTY(m_state_flags,  2,  IS_DESELECT_WHEEL_SCHEDULED,       IsDeselectWheelScheduled,       SetIsDeselectWheelScheduled);
    BITMASK_PROPERTY(m_state_flags,  3,  IS_SELECT_ALL_WHEELS_SCHEDULED,    IsSelectAllWheelsScheduled,     SetIsSelectAllWheelsScheduled);
    BITMASK_PROPERTY(m_state_flags,  4,  IS_DESELECT_ALL_WHEELS_SCHEDULED,  IsDeselectAllWheelsScheduled,   SetIsDeselectAllWheelsScheduled);
    inline bool IsAnyWheelSelectionChangeScheduled() const
    { 
        return IsSelectWheelScheduled() || IsDeselectWheelScheduled() || IsSelectAllWheelsScheduled() || IsDeselectAllWheelsScheduled();
    }
    inline void ResetAllScheduledWheelSelectionChanges()
    {
        BITMASK_SET_0(m_state_flags, IS_SELECT_WHEEL_SCHEDULED | IS_DESELECT_WHEEL_SCHEDULED | IS_SELECT_ALL_WHEELS_SCHEDULED | IS_DESELECT_ALL_WHEELS_SCHEDULED);
    }
    // --- Flares ---
    BITMASK_PROPERTY(m_state_flags,  5,  IS_SELECT_FLARE_SCHEDULED,         IsSelectFlareScheduled,         SetIsSelectFlareScheduled);
    BITMASK_PROPERTY(m_state_flags,  6,  IS_DESELECT_FLARE_SCHEDULED,       IsDeselectFlareScheduled,       SetIsDeselectFlareScheduled);
    BITMASK_PROPERTY(m_state_flags,  7,  IS_SELECT_ALL_FLARES_SCHEDULED,    IsSelectAllFlaresScheduled,     SetIsSelectAllFlaresScheduled);
    BITMASK_PROPERTY(m_state_flags,  8,  IS_DESELECT_ALL_FLARES_SCHEDULED,  IsDeselectAllFlaresScheduled,   SetIsDeselectAllFlaresScheduled);
    inline bool IsAnyFlareSelectionChangeScheduled() const
    { 
        return IsSelectFlareScheduled() || IsDeselectFlareScheduled() || IsSelectAllFlaresScheduled() || IsDeselectAllFlaresScheduled();
    }
    inline void ResetAllScheduledFlareSelectionChanges()
    {
        BITMASK_SET_0(m_state_flags, IS_SELECT_FLARE_SCHEDULED | IS_DESELECT_FLARE_SCHEDULED | IS_SELECT_ALL_FLARES_SCHEDULED | IS_DESELECT_ALL_FLARES_SCHEDULED);
    }

    
    //void AS_OnEnter_InitializeOrRestoreGui_UGLY();

	void AS_UpdateMainLoop_UGLY();
	bool AS_WasExitLoopRequested_UGLY();

	void AS_OnExit_HideGui_UGLY();
	void AS_OnExit_ClearExitRequest_UGLY();

	void AS_RegisterUserCommandCallback_UGLY(AngelScript::asIScriptObject* object, std::string method_name);

	// ===== Command interface handlers ===== //

    // File management
	void AS_HandleCommandSaveRigFile_UGLY();
	void AS_HandleCommandCloseCurrentRig_UGLY();
    void AS_HandleCommandCreateNewEmptyRig_UGLY();
	void AS_HandleCommandCurrentRigDeleteSelectedNodes_UGLY();
	void AS_HandleCommandCurrentRigDeleteSelectedBeams_UGLY();
	void AS_HandleCommandQuitRigEditor_UGLY();
	void AS_HandleCommandShowRigPropertiesWindow_UGLY();
	void AS_HandleCommandSaveContentOfRigPropertiesWindow_UGLY();

    // Land vehicle window
	void AS_HandleCommandShowLandVehiclePropertiesWindow_UGLY();
	void AS_HandleCommandSaveLandVehiclePropertiesWindowData_UGLY();

	// Rig management
	bool AS_LoadRigDefFile(std::string directory, std::string filename);
	void AS_SaveRigDefFile(std::string directory, std::string filename);

    // ========== END Angelscript interface ==========
    
private:
	
	void HideAllNodeBeamGuiPanels();
    void HideAllWheelGuiPanels();

    void OnNewRigCreatedOrLoaded(Ogre::SceneNode* parent_scene_node);

	void InvokeAngelScriptUserCommandCallback(IMain::UserCommand command);



    unsigned int         m_state_flags;
	bool                 m_exit_loop_requested;

	// GUI
	std::unique_ptr<GUI::RigEditorMenubar>                      m_gui_menubar;
	std::unique_ptr<GUI::RigEditorDeleteMenu>                   m_gui_delete_menu;
	std::unique_ptr<GUI::RigEditorNodePanel>                    m_nodes_panel;
	std::unique_ptr<GUI::RigEditorBeamsPanel>                   m_beams_panel;
	std::unique_ptr<GUI::RigEditorHydrosPanel>                  m_hydros_panel;
	std::unique_ptr<GUI::RigEditorShocksPanel>                  m_shocks_panel;
    std::unique_ptr<GUI::RigEditorFlaresPanel>                  m_flares_panel;
	std::unique_ptr<GUI::RigEditorShocks2Panel>                 m_shocks2_panel;
	std::unique_ptr<GUI::RigEditorCommands2Panel>               m_commands2_panel;
    std::unique_ptr<GUI::RigEditorFlaresListPanel>              m_flares_list_panel;
    std::unique_ptr<GUI::RigEditorMeshWheels2Panel>             m_meshwheels2_panel;
    std::unique_ptr<GUI::RigEditorFlexBodyWheelsPanel>          m_flexbodywheels_panel;
    std::unique_ptr<GUI::RigEditorRigPropertiesWindow>          m_gui_rig_properties_window;
	std::unique_ptr<GUI::RigEditorLandVehiclePropertiesWindow>  m_gui_land_vehicle_properties_window;

	// AngelScript
	//AngelScriptCallbackSocket m_as_user_command_callback;
	*/
};



void CreateRigEditorGlobalInstance();

Main* GetRigEditorGlobalInstance();

} // namespace RigEditor

} // namespace RoR

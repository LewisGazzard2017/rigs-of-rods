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
	@file   RigEditor_IMain.h
	@date   08/2014
	@author Petr Ohlidal
*/

#pragma once

#include "RigEditor_ForwardDeclarations.h"

namespace RoR
{

namespace RigEditor
{

/** Command interface to RigEditor */

// RIG_EDITOR_PYTHON: 
//    TO BE REMOVED... Filled with empty bodies just to compile.
class IMain
{
public:

	enum UserCommand
	{
		USER_COMMAND_SHOW_DIALOG_OPEN_RIG_FILE,
		USER_COMMAND_SHOW_DIALOG_SAVE_RIG_FILE_AS,
		USER_COMMAND_SAVE_RIG_FILE,
		USER_COMMAND_CLOSE_CURRENT_RIG,
		USER_COMMAND_CREATE_NEW_EMPTY_RIG,
		USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_NODES,
		USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_BEAMS,
		USER_COMMAND_QUIT_RIG_EDITOR,
		USER_COMMAND_SHOW_RIG_PROPERTIES_WINDOW,
		USER_COMMAND_SAVE_CONTENT_OF_RIG_PROPERTIES_WINDOW,
		USER_COMMAND_SHOW_LAND_VEHICLE_PROPERTIES_WINDOW,
		USER_COMMAND_SAVE_LAND_VEHICLE_PROPERTIES_WINDOW_DATA,
		USER_COMMAND_SHOW_HELP_WINDOW,
		USER_COMMAND_INVALID
	};

    // File management
	virtual void CommandShowDialogOpenRigFile() {};
	virtual void CommandShowDialogSaveRigFileAs() {};
	virtual void CommandSaveRigFile() {};
	virtual void CommandCloseCurrentRig() {};
    virtual void CommandCreateNewEmptyRig() {};

	virtual void CommandCurrentRigDeleteSelectedNodes() {};

	virtual void CommandCurrentRigDeleteSelectedBeams() {};

	virtual void CommandQuitRigEditor() {};

	virtual void CommandShowRigPropertiesWindow() {};

	virtual void CommandSaveContentOfRigPropertiesWindow() {};

	virtual void CommandShowLandVehiclePropertiesWindow() {};
    
	virtual void CommandSaveLandVehiclePropertiesWindowData() {};

	virtual void CommandShowHelpWindow() {};

	// Rig updaters
	virtual void CommandRigSelectedNodesUpdateAttributes     (const RigAggregateNodesData*      data) {};
	virtual void CommandRigSelectedPlainBeamsUpdateAttributes(const RigAggregatePlainBeamsData* data) {};
	virtual void CommandRigSelectedShocksUpdateAttributes    (const RigAggregateShocksData*     data) {};
	virtual void CommandRigSelectedShocks2UpdateAttributes   (const RigAggregateShocks2Data*    data) {};
	virtual void CommandRigSelectedHydrosUpdateAttributes    (const RigAggregateHydrosData*     data) {};
	virtual void CommandRigSelectedCommands2UpdateAttributes (const RigAggregateCommands2Data*  data) {};

	// Land vehicle wheel list
	virtual void CommandScheduleSetWheelSelected (LandVehicleWheel* wheel_ptr, int wheel_index, bool state_selected) {};
	virtual void CommandSetWheelHovered          (LandVehicleWheel* wheel_ptr, int wheel_index, bool state_hovered) {};
	virtual void CommandScheduleSetAllWheelsSelected (bool state_selected) {};
	virtual void CommandSetAllWheelsHovered          (bool state_selected) {};
	
    // Flares list
    virtual void CommandShowFlaresList() {};
	virtual void CommandScheduleSetFlareSelected (Flare* flare_ptr, int flare_index, bool state_selected) {};
	virtual void CommandSetFlareHovered          (Flare* flare_ptr, int flare_index, bool state_hovered) {};
	virtual void CommandScheduleSetAllFlaresSelected(bool state_selected) {};
	virtual void CommandSetAllFlaresHovered         (bool state_selected) {};

    // GETTERS

    virtual RigEditor::Node*      GetCurrentRigLastSelectedNode() { return nullptr; };

    // FACTORIES

    virtual PointListDynamicMesh*    CreateInstanceOfPointListDynamicMesh(float point_size, size_t estimate_point_count) { return nullptr; };
    virtual LineListDynamicMesh*     CreateInstanceOfLineListDynamicMesh(size_t estimate_line_count) { return nullptr; };
};

} // namespace RigEditor

} // namespace RoR

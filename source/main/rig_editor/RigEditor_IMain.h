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
	virtual void CommandShowDialogOpenRigFile() = 0;
	virtual void CommandShowDialogSaveRigFileAs() = 0;
	virtual void CommandSaveRigFile() = 0;
	virtual void CommandCloseCurrentRig() = 0;
    virtual void CommandCreateNewEmptyRig() = 0;

	virtual void CommandCurrentRigDeleteSelectedNodes() = 0;

	virtual void CommandCurrentRigDeleteSelectedBeams() = 0;

	virtual void CommandQuitRigEditor() = 0;

	virtual void CommandShowRigPropertiesWindow() = 0;

	virtual void CommandSaveContentOfRigPropertiesWindow() = 0;

	virtual void CommandShowLandVehiclePropertiesWindow() = 0;
    
	virtual void CommandSaveLandVehiclePropertiesWindowData() = 0;

	virtual void CommandShowHelpWindow() = 0;

	// Rig updaters
	virtual void CommandRigSelectedNodesUpdateAttributes     (const RigAggregateNodesData*      data) = 0;
	virtual void CommandRigSelectedPlainBeamsUpdateAttributes(const RigAggregatePlainBeamsData* data) = 0;
	virtual void CommandRigSelectedShocksUpdateAttributes    (const RigAggregateShocksData*     data) = 0;
	virtual void CommandRigSelectedShocks2UpdateAttributes   (const RigAggregateShocks2Data*    data) = 0;
	virtual void CommandRigSelectedHydrosUpdateAttributes    (const RigAggregateHydrosData*     data) = 0;
	virtual void CommandRigSelectedCommands2UpdateAttributes (const RigAggregateCommands2Data*  data) = 0;

	// Land vehicle wheel list
	virtual void CommandScheduleSetWheelSelected (LandVehicleWheel* wheel_ptr, int wheel_index, bool state_selected) = 0;
	virtual void CommandSetWheelHovered          (LandVehicleWheel* wheel_ptr, int wheel_index, bool state_hovered) = 0;
	virtual void CommandScheduleSetAllWheelsSelected (bool state_selected) = 0;
	virtual void CommandSetAllWheelsHovered          (bool state_selected) = 0;
	
    // Flares list
    virtual void CommandShowFlaresList() = 0;
	virtual void CommandScheduleSetFlareSelected (Flare* flare_ptr, int flare_index, bool state_selected) = 0;
	virtual void CommandSetFlareHovered          (Flare* flare_ptr, int flare_index, bool state_hovered) = 0;
	virtual void CommandScheduleSetAllFlaresSelected(bool state_selected) = 0;
	virtual void CommandSetAllFlaresHovered         (bool state_selected) = 0;

    // GETTERS

    virtual RigEditor::Node*      GetCurrentRigLastSelectedNode() = 0;

    // FACTORIES

    virtual PointListDynamicMesh*    CreateInstanceOfPointListDynamicMesh(float point_size, size_t estimate_point_count) = 0;
    virtual LineListDynamicMesh*     CreateInstanceOfLineListDynamicMesh(size_t estimate_line_count) = 0;
};

} // namespace RigEditor

} // namespace RoR

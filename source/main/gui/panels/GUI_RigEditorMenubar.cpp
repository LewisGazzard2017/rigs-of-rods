/*
    This source file is part of Rigs of Rods
    Copyright 2013-2016 Petr Ohlidal & contributors

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
    @file   GUI_RigEditorMenubar.cpp
    @author Petr Ohlidal
    @date   08/2014
*/

#include "GUI_RigEditorMenubar.h"

#include "RigEditor_GuiPopupWheelsList.h"
#include "RigEditor_GuiPopupFlaresList.h"

#include <MyGUI.h>

using namespace RoR;
using namespace GUI;

RigEditorMenubar::RigEditorMenubar(RigEditor::IMain* rig_editor_interface)
{
    m_rig_editor_interface = rig_editor_interface;

    m_file_popup_item_import_truckfile    ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::ImportTruckfileItemClicked);
    m_file_popup_item_export_truckfile_as ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::ExportTruckfileAsItemClicked);
    m_file_popup_item_open_json           ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::OpenJsonItemClicked);
    m_file_popup_item_save_json_as        ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::SaveJsonAsItemClicked);
    m_file_popup_item_close               ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::CloseRigItemClicked);
    m_file_popup_item_quit                ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::QuitEditorItemClicked);
    m_file_popup_item_properties          ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::RigPropertiesItemClicked);
    m_file_popup_item_land_properties     ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::LandVehiclePropertiesItemClicked);
    m_file_popup_item_create_empty        ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::CreateEmptyRigItemClicked);
    m_menubar_item_help                   ->eventMouseButtonClick += MyGUI::newDelegate(this, &RigEditorMenubar::MenubarItemHelpClicked);

    m_wheels_list = std::unique_ptr<RigEditor::GuiPopupWheelsList>(
        new RigEditor::GuiPopupWheelsList(
            rig_editor_interface, m_wheels_popup,
            m_wheels_popup_item_select_all, m_wheels_popup_item_deselect_all)
        );

    m_flares_list = std::unique_ptr<RigEditor::GuiPopupFlaresList>(
		new RigEditor::GuiPopupFlaresList(
			rig_editor_interface, m_flares_popup,
			m_flares_popup_item_select_all, m_flares_popup_item_deselect_all)
		);
}

void RigEditorMenubar::Show()
{
    m_rig_editor_menubar->setVisible(true);
}

void RigEditorMenubar::Hide()
{
    m_rig_editor_menubar->setVisible(false);
}

void RigEditorMenubar::StretchWidthToScreen()
{
    MyGUI::IntSize parentSize = m_rig_editor_menubar->getParentSize();
    m_rig_editor_menubar->setSize(parentSize.width, m_rig_editor_menubar->getHeight());
}

void RigEditorMenubar::UpdateLandVehicleWheelsList(std::vector<RigEditor::LandVehicleWheel*> & list)
{
    m_wheels_list->UpdateWheelsList(list);
}

void RigEditorMenubar::ClearLandVehicleWheelsList()
{
    m_wheels_list->ClearWheelsList();
}

void RigEditorMenubar::UpdateFlaresList(std::vector<RigEditor::Flare*> & list)
{
	m_flares_list->UpdateFlaresList(list);
}

void RigEditorMenubar::ClearFlaresList()
{
	m_flares_list->ClearFlaresList();
}

// ============================================================================
// Event handlers
// ============================================================================

void RigEditorMenubar::ImportTruckfileItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowDialogOpenRigFile();
}

void RigEditorMenubar::ExportTruckfileAsItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowDialogSaveRigFileAs();
}

void RigEditorMenubar::OpenJsonItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowDialogOpenJsonProject();
}

void RigEditorMenubar::SaveJsonAsItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowDialogSaveJsonProjectAs();
}

void RigEditorMenubar::CloseRigItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandCloseCurrentRig();
}

void RigEditorMenubar::QuitEditorItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandQuitRigEditor();
}

void RigEditorMenubar::RigPropertiesItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowRigPropertiesWindow();
}

void RigEditorMenubar::LandVehiclePropertiesItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowLandVehiclePropertiesWindow();
}

void RigEditorMenubar::MenubarItemHelpClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandShowHelpWindow();
}

void RigEditorMenubar::CreateEmptyRigItemClicked(MyGUI::Widget* sender)
{
    m_rig_editor_interface->CommandCreateNewEmptyRig();
}

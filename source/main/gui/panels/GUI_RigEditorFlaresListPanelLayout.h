#pragma once

// ----------------------------------------------------------------------------
// Generated by MyGUI's LayoutEditor using RoR's code templates.
// Find the templates at [repository]/tools/MyGUI_LayoutEditor/
//
// IMPORTANT:
// Do not modify this code directly. Create a derived class instead.
// ----------------------------------------------------------------------------

#include "ForwardDeclarations.h"
#include "BaseLayout.h"

namespace RoR
{

namespace GUI
{
	
ATTRIBUTE_CLASS_LAYOUT(RigEditorFlaresListPanelLayout, "rig_editor_flares_list.layout");
class RigEditorFlaresListPanelLayout : public wraps::BaseLayout
{

public:

	RigEditorFlaresListPanelLayout(MyGUI::Widget* _parent = nullptr);
	virtual ~RigEditorFlaresListPanelLayout();

protected:

	//%LE Widget_Declaration list start
	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_flares_list_panel, "flares_list_panel");
	MyGUI::Window* m_flares_list_panel;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_display_checkbox, "display_checkbox");
	MyGUI::Button* m_display_checkbox;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_display_label, "display_label");
	MyGUI::TextBox* m_display_label;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_items_listbox, "items_listbox");
	MyGUI::ListBox* m_items_listbox;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_select_all_button, "select_all_button");
	MyGUI::Button* m_select_all_button;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_deselect_all_button, "deselect_all_button");
	MyGUI::Button* m_deselect_all_button;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_add_new_button, "add_new_button");
	MyGUI::Button* m_add_new_button;

	ATTRIBUTE_FIELD_WIDGET_NAME(RigEditorFlaresListPanelLayout, m_select_by_type_combobox, "select_by_type_combobox");
	MyGUI::ComboBox* m_select_by_type_combobox;

	//%LE Widget_Declaration list end
};

} // namespace GUI

} // namespace RoR


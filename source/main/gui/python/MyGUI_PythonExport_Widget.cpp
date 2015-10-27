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
	@file
	@date   10/2015
	@author Petr Ohlidal
*/

#pragma once

#include "MyGUI_PythonExport.h"

#include <boost/python/detail/wrap_python.hpp>
	// Replaces #include <Python.h>, recommended in Boost docs
	// Must be included before any OS headers - Python limitation

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python.hpp>

#include "MyGUI.h"
#include "PythonBindingUtils.h"

using namespace RoR;
using namespace boost::python;

// ================================================================================
// Widget typecasting extension methods
// ================================================================================

static MyGUI::Button*     PY_Widget_CastType_Button   (MyGUI::Widget* _this) { return _this->castType<MyGUI::Button>(); }
static MyGUI::EditBox*    PY_Widget_CastType_EditBox  (MyGUI::Widget* _this) { return _this->castType<MyGUI::EditBox>(); }
static MyGUI::ComboBox*   PY_Widget_CastType_ComboBox (MyGUI::Widget* _this) { return _this->castType<MyGUI::ComboBox>(); }
static MyGUI::TextBox*    PY_Widget_CastType_TextBox  (MyGUI::Widget* _this) { return _this->castType<MyGUI::TextBox>(); }
static MyGUI::MenuBar*    PY_Widget_CastType_MenuBar  (MyGUI::Widget* _this) { return _this->castType<MyGUI::MenuBar>(); }
static MyGUI::PopupMenu*  PY_Widget_CastType_PopupMenu(MyGUI::Widget* _this) { return _this->castType<MyGUI::PopupMenu>(); }
static MyGUI::Window*     PY_Widget_CastType_Window   (MyGUI::Widget* _this) { return _this->castType<MyGUI::Window>(); }

// ================================================================================
// Export
// ================================================================================

// static
void MyGUI_PythonExport::ExportClassWidget()
{
	// boost::noncopyable is necessary because MyGUI::Widget has protected destructor.

	class_<MyGUI::Widget, boost::noncopyable>("MyGUI_Widget", no_init)
		// Event handles
		.def_readonly("event_mouse_set_focus",    &MyGUI::Widget::eventMouseSetFocus)
		.def_readonly("event_mouse_lost_focus",   &MyGUI::Widget::eventMouseLostFocus)
		.def_readonly("event_mouse_button_click", &MyGUI::Widget::eventMouseButtonClick)
		// Typecasts
		.def("cast_type_Button",     &PY_Widget_CastType_Button,     return_value_policy<manage_new_object>())
		.def("cast_type_EditBox",    &PY_Widget_CastType_EditBox,    return_value_policy<manage_new_object>())
		.def("cast_type_ComboBox",   &PY_Widget_CastType_ComboBox,   return_value_policy<manage_new_object>())
		.def("cast_type_TextBox",    &PY_Widget_CastType_TextBox,    return_value_policy<manage_new_object>())
		.def("cast_type_MenuBar",    &PY_Widget_CastType_MenuBar,    return_value_policy<manage_new_object>())
		.def("cast_type_PopupMenu",  &PY_Widget_CastType_PopupMenu,  return_value_policy<manage_new_object>())
		.def("cast_type_Window",     &PY_Widget_CastType_Window,     return_value_policy<manage_new_object>())
		// Methods
		.def("find_widget",      &MyGUI::Widget::findWidget,   return_value_policy<reference_existing_object>())
		.def("is_root_widget",   &MyGUI::Widget::isRootWidget)
		.def("get_parent",       &MyGUI::Widget::getParent,    return_value_policy<reference_existing_object>())
		.def("is_visible",       &MyGUI::Widget::getVisible)
		.def("set_visible",      &MyGUI::Widget::setVisible)
		.def("get_name",         &MyGUI::Widget::getName,      return_value_policy<copy_const_reference>())
		.def("is_enabled",       &MyGUI::Widget::getEnabled)
		.def("set_enabled",      &MyGUI::Widget::setEnabled)
		;
}

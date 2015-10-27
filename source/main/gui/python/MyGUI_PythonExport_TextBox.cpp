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
// Wrappers
// ================================================================================

// Converts return value from UString to std::string
static std::string PY_MyGUI_TextBox_GetCaptionUtf8(MyGUI::TextBox* _this)
{
	return _this->getCaption().asUTF8();
}

// Converts parameter from Python "str" (-> automatic std::string) to UString
static void PY_MyGUI_TextBox_SetCaptionUtf8(MyGUI::TextBox* _this, std::string caption)
{
	_this->setCaption(caption);
}

// ================================================================================
// Export
// ================================================================================

// static
void MyGUI_PythonExport::ExportClassTextBox()
{
	// boost::noncopyable is necessary because MyGUI::Widget has protected destructor.

	class_<MyGUI::TextBox, bases<MyGUI::Widget>, boost::noncopyable>("MyGUI_TextBox", no_init)
		.def("get_caption",     &PY_MyGUI_TextBox_GetCaptionUtf8) // Returns a non-reference, no need for return_policy
		.def("set_caption",     &PY_MyGUI_TextBox_SetCaptionUtf8)
		.def("get_font_name",   &MyGUI::TextBox::getFontName, return_value_policy<reference_existing_object>())
		.def("set_font_name",   &MyGUI::TextBox::setFontName)
		.def("get_text_colour", &MyGUI::TextBox::getTextColour, return_value_policy<reference_existing_object>())
		.def("set_text_colour", &MyGUI::TextBox::setTextColour)
		;
}

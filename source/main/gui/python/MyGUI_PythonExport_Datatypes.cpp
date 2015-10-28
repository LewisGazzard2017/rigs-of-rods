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

static MyGUI::Colour PY_MyGUI_Colour_Constructor_RGBA(float r, float g, float b, float a)
{
	return MyGUI::Colour(r,g,b,a);
}

static MyGUI::Colour PY_MyGUI_Colour_Constructor_RGB(float r, float g, float b)
{
	return MyGUI::Colour(r,g,b);
}

// ================================================================================
// Export
// ================================================================================

// static
void MyGUI_PythonExport::ExportDatatypes()
{
	class_<MyGUI::Colour>("MyGUI_Colour", init<>()) // Default constructor, no params
		.def(init<float, float, float, float>()) // RGBA constructor
		.def_readwrite("red",   &MyGUI::Colour::red)
		.def_readwrite("green", &MyGUI::Colour::green)
		.def_readwrite("blue",  &MyGUI::Colour::blue)
		.def_readwrite("alpha", &MyGUI::Colour::alpha)
		;

	PYTHON_REGISTER_VECTOR_SUITE(MyGUI::VectorWidgetPtr, "MyGUI_VectorWidgetPtr")
}

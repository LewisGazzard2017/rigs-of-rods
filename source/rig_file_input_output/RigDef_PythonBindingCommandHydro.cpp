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
	Separated PythonBinding::Export*() method to aid compilation.
	Technique recommended in boost docs: http://www.boost.org/doc/libs/1_59_0/libs/python/doc/tutorial/doc/html/python/techniques.html#python.reducing_compiling_time
*/

#include "RigDef_PythonBinding.h"

#include <boost/python/detail/wrap_python.hpp>
	// Replaces #include <Python.h>, recommended in Boost docs
	// Must be included before any OS heders - Python limitation
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>

#include "RigDef_Node.h"
#include "RigDef_File.h"

using namespace boost::python;
using namespace RigDef;

void PythonBinding::ExportCommandHydro()
{
	class_<RigDef::Command2>("CommandHydro")
		.add_property("node_1", &Command2::PY_GetNode1, &Command2::PY_SetNode1)
		.add_property("node_2", &Command2::PY_GetNode2, &Command2::PY_SetNode2)

		.add_property("option_i_invisible",         &Command2::HasOption_i_Invisible,       &Command2::SetOption_i_Invisible      )
		.add_property("option_r_rope",              &Command2::HasOption_r_Rope,            &Command2::SetOption_r_Rope           )
		.add_property("option_c_auto_center",       &Command2::HasOption_c_AutoCenter,      &Command2::SetOption_c_AutoCenter     )
		.add_property("option_f_not_faster",        &Command2::HasOption_f_NotFaster,       &Command2::SetOption_f_NotFaster      )
		.add_property("option_p_press_once",        &Command2::HasOption_p_PressOnce,       &Command2::SetOption_p_PressOnce      )
		.add_property("option_o_press_once_center", &Command2::HasOption_o_PressOnceCenter, &Command2::SetOption_o_PressOnceCenter)

		.def_readwrite("shorten_rate"             , &Command2::shorten_rate)
		.def_readwrite("lengthen_rate"            , &Command2::lengthen_rate)
		.def_readwrite("max_contraction"          , &Command2::max_contraction)
		.def_readwrite("max_extension"            , &Command2::max_extension)
		.def_readwrite("contract_key"             , &Command2::contract_key)
		.def_readwrite("extend_key"               , &Command2::extend_key)
		.def_readwrite("description"              , &Command2::description)
		.def_readwrite("affect_engine"            , &Command2::affect_engine)
		.def_readwrite("needs_engine"             , &Command2::needs_engine)

		.def_readwrite("detacher_group"           , &Command2::detacher_group)
		.def_readwrite("beam_preset"              , &Command2::beam_defaults)
		.def_readwrite("inertia_preset"           , &Command2::inertia_defaults)
		;

	class_<std::vector<RigDef::Command2> >("CommandHydroVector")
		.def(vector_indexing_suite< std::vector<RigDef::Command2> >())
		;

	class_<RigDef::Command2GroupWithPreset>("CommandHydroGroupWithPreset")
		.def_readwrite("preset",           &Command2GroupWithPreset::preset)
		.def_readwrite("command_hydros",   &Command2GroupWithPreset::commands)
		;

	class_<std::vector<RigDef::Command2GroupWithPreset> >("CommandHydroGroupVector")
		.def(vector_indexing_suite< std::vector<RigDef::Command2GroupWithPreset> >())
		;
}

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

void PythonBinding::ExportHydraulics()
{
    PYTHON_REGISTER_STD_VECTOR(RigDef::Command2, "CommandHydroVector")
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

    PYTHON_REGISTER_STD_VECTOR(RigDef::Command2GroupWithPreset, "CommandHydroGroupVector")
	class_<RigDef::Command2GroupWithPreset>("CommandHydroGroupWithPreset")
		.def_readwrite("preset",           &Command2GroupWithPreset::preset)
		.def_readwrite("command_hydros",   &Command2GroupWithPreset::commands)
		;
        
    PYTHON_REGISTER_STD_VECTOR(RigDef::Hydro, "SteeringHydroVector")    
    class_<RigDef::Hydro>("SteeringHydro")
        .add_property("node_1", &Hydro::PY_GetNode1, &Hydro::PY_SetNode1)
        .add_property("node_2", &Hydro::PY_GetNode2, &Hydro::PY_SetNode2)
        
        // Option constants (for adding)
        .def_readonly("OPTION_n_NORMAL",                    &Hydro::OPTION_n_NORMAL                   )
        .def_readonly("OPTION_i_INVISIBLE",                 &Hydro::OPTION_i_INVISIBLE                )
        .def_readonly("OPTION_s_DISABLE_ON_HIGH_SPEED",     &Hydro::OPTION_s_DISABLE_ON_HIGH_SPEED    )
        .def_readonly("OPTION_a_INPUT_AILERON",             &Hydro::OPTION_a_INPUT_AILERON            )
        .def_readonly("OPTION_r_INPUT_RUDDER",              &Hydro::OPTION_r_INPUT_RUDDER             )
        .def_readonly("OPTION_e_INPUT_ELEVATOR",            &Hydro::OPTION_e_INPUT_ELEVATOR           )
        .def_readonly("OPTION_u_INPUT_AILERON_ELEVATOR",    &Hydro::OPTION_u_INPUT_AILERON_ELEVATOR   )
        .def_readonly("OPTION_v_INPUT_InvAILERON_ELEVATOR", &Hydro::OPTION_v_INPUT_InvAILERON_ELEVATOR)
        .def_readonly("OPTION_x_INPUT_AILERON_RUDDER",      &Hydro::OPTION_x_INPUT_AILERON_RUDDER     )
        .def_readonly("OPTION_y_INPUT_InvAILERON_RUDDER",   &Hydro::OPTION_y_INPUT_InvAILERON_RUDDER  )
        .def_readonly("OPTION_g_INPUT_ELEVATOR_RUDDER",     &Hydro::OPTION_g_INPUT_ELEVATOR_RUDDER    )
        .def_readonly("OPTION_h_INPUT_InvELEVATOR_RUDDER",  &Hydro::OPTION_h_INPUT_InvELEVATOR_RUDDER )
        
        // Option read-only properties
        .add_property("has_option_i_invisible",                 &Hydro::HasFlag_i)
        .add_property("has_option_s_disable_on_high_speed",     &Hydro::HasFlag_s)
        .add_property("has_option_a_input_aileron",             &Hydro::HasFlag_a)
        .add_property("has_option_r_input_rudder",              &Hydro::HasFlag_r)
        .add_property("has_option_e_input_elevator",            &Hydro::HasFlag_e)
        .add_property("has_option_u_input_aileron_elevator",    &Hydro::HasFlag_u)
        .add_property("has_option_v_input_invaileron_elevator", &Hydro::HasFlag_v)
        .add_property("has_option_x_input_aileron_rudder",      &Hydro::HasFlag_x)
        .add_property("has_option_y_input_invaileron_rudder",   &Hydro::HasFlag_y)
        .add_property("has_option_g_input_elevator_rudder",     &Hydro::HasFlag_g)
        .add_property("has_option_h_input_invelevator_rudder",  &Hydro::HasFlag_h)
        
        // Option setter
        .def("add_option", &Hydro::AddFlag)
        
        // Attributes
        .def_readwrite("lenghtening_factor", &Hydro::lenghtening_factor)
        .def_readwrite("detacher_group",     &Hydro::detacher_group)
        .def_readwrite("inertia",            &Hydro::inertia)
        .def_readwrite("inertia_defaults",   &Hydro::inertia_defaults)
        .def_readwrite("beam_defaults",      &Hydro::beam_defaults)
        ;
}

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

void PythonBinding::ExportTrigger()
{
    enum_<RigDef::Trigger::EngineTrigger::Function>("EngineTriggerFunction")
        .value("CLUTCH",      Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_CLUTCH     )
        .value("BRAKE",       Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_BRAKE      )
        .value("ACCELERATOR", Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_ACCELERATOR)
        .value("RPM_CONTROL", Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_RPM_CONTROL)
        .value("SHIFT_UP",    Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_SHIFT_UP   )
        .value("SHIFT_DOWN",  Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_SHIFT_DOWN )
        .value("INVALID",     Trigger::EngineTrigger::ENGINE_TRIGGER_FUNCTION_INVALID    )
        ;


    class_<RigDef::Trigger::EngineTrigger>("EngineTrigger")   
        .def_readwrite("function",    &Trigger::EngineTrigger::function)
        .def_readwrite("motor_index", &Trigger::EngineTrigger::motor_index)
        ;


    class_<RigDef::Trigger::CommandKeyTrigger>("CommandKeyTrigger")
        .def_readwrite("contraction_trigger_key", &Trigger::CommandKeyTrigger::contraction_trigger_key)
        .def_readwrite("extension_trigger_key",   &Trigger::CommandKeyTrigger::extension_trigger_key)
        ;


    class_<RigDef::Trigger::HookToggleTrigger>("HookToggleTrigger")   
        .def_readwrite("contraction_trigger_hookgroup_id", &Trigger::HookToggleTrigger::contraction_trigger_hookgroup_id)
        .def_readwrite("extension_trigger_hookgroup_id",   &Trigger::HookToggleTrigger::extension_trigger_hookgroup_id)
        ;


    class_<RigDef::Trigger>("Trigger")
        .add_property("node_1", &Trigger::PY_GetNode1, &Trigger::PY_SetNode1)
        .add_property("node_2", &Trigger::PY_GetNode2, &Trigger::PY_SetNode2)

        .add_property("use_option_i_invisible",             &Trigger::HasFlag_i_Invisible,         &Trigger::SetFlag_i_Invisible        )
        .add_property("use_option_c_command_style",         &Trigger::HasFlag_c_CommandStyle,      &Trigger::SetFlag_c_CommandStyle     )
        .add_property("use_option_x_start_off",             &Trigger::HasFlag_x_StartDisabled,     &Trigger::SetFlag_x_StartDisabled    )
        .add_property("use_option_b_block_keys",            &Trigger::HasFlag_b_KeyBlocker,        &Trigger::SetFlag_b_KeyBlocker       )
        .add_property("use_option_b_block_triggers",        &Trigger::HasFlag_B_TriggerBlocker,    &Trigger::SetFlag_B_TriggerBlocker   )
        .add_property("use_option_a_inv_block_triggers",    &Trigger::HasFlag_A_InvTriggerBlocker, &Trigger::SetFlag_A_InvTriggerBlocker)
        .add_property("use_option_s_switch_cmd_num",        &Trigger::HasFlag_s_CmdNumSwitch,      &Trigger::SetFlag_s_CmdNumSwitch     )
        .add_property("use_option_h_unlock_hookgroups_key", &Trigger::HasFlag_h_UnlocksHookGroup,  &Trigger::SetFlag_h_UnlocksHookGroup )
        .add_property("use_option_h_lock_hookgroups_key",   &Trigger::HasFlag_H_LocksHookGroup,    &Trigger::SetFlag_H_LocksHookGroup   )
        .add_property("use_option_t_continuous",            &Trigger::HasFlag_t_Continuous,        &Trigger::SetFlag_t_Continuous       )
        .add_property("use_option_e_engine_trigger",        &Trigger::HasFlag_E_EngineTrigger,     &Trigger::SetFlag_E_EngineTrigger    )

        .def("is_hook_toggle_trigger",      &Trigger::IsHookToggleTrigger)
        .def("is_trigger_blocker_any_type", &Trigger::IsTriggerBlockerAnyType)

        .def("set_engine_trigger",          &Trigger::SetEngineTrigger)
        .def("get_engine_trigger",          &Trigger::GetEngineTrigger)

        .def("set_command_key_trigger",     &Trigger::SetCommandKeyTrigger)
        .def("get_command_key_trigger",     &Trigger::GetCommandKeyTrigger)

        .def("set_hook_toggle_trigger",     &Trigger::SetHookToggleTrigger)
        .def("get_hook_toggle_trigger",     &Trigger::GetHookToggleTrigger)

        .def_readwrite("contraction_trigger_limit", &Trigger::contraction_trigger_limit)
        .def_readwrite("expansion_trigger_limit",   &Trigger::expansion_trigger_limit)
        .def_readwrite("options",                   &Trigger::options)
        .def_readwrite("boundary_timer",            &Trigger::boundary_timer)
        .def_readwrite("beam_defaults",             &Trigger::beam_defaults)
        .def_readwrite("detacher_group",            &Trigger::detacher_group)
        .def_readwrite("shortbound_trigger_action", &Trigger::shortbound_trigger_action)
        .def_readwrite("longbound_trigger_action",  &Trigger::longbound_trigger_action)
        ;

}

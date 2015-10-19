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

void PythonBinding::ExportPowertrain()
{
	class_<RigDef::DefaultInertia>("InertiaPreset")
		.def_readwrite("start_delay_factor"    , &DefaultInertia::start_delay_factor)
		.def_readwrite("stop_delay_factor"     , &DefaultInertia::stop_delay_factor)
		.def_readwrite("start_function"        , &DefaultInertia::start_function)
		.def_readwrite("stop_function "        , &DefaultInertia::stop_function )
		;

	class_<RigDef::OptionalInertia>("OptionalInertia")
		.def_readwrite("start_delay_factor"      , &OptionalInertia::start_delay_factor)
		.def_readwrite("stop_delay_factor"       , &OptionalInertia::stop_delay_factor)
		.def_readwrite("start_function"          , &OptionalInertia::start_function)
		.def_readwrite("stop_function "          , &OptionalInertia::stop_function )
		.def_readwrite("_start_delay_factor_set" , &OptionalInertia::_start_delay_factor_set)
		.def_readwrite("_stop_delay_factor_set"  , &OptionalInertia::_stop_delay_factor_set)
		;

	class_<RigDef::Engine>("Engine")
		.def_readwrite("shift_down_rpm" ,     &Engine::shift_down_rpm)
		.def_readwrite("shift_up_rpm" ,       &Engine::shift_up_rpm)
		.def_readwrite("torque" ,             &Engine::torque)
		.def_readwrite("global_gear_ratio" ,  &Engine::global_gear_ratio)
		.def_readwrite("reverse_gear_ratio" , &Engine::reverse_gear_ratio)
		.def_readwrite("neutral_gear_ratio" , &Engine::neutral_gear_ratio)
		.def_readwrite("gear_ratios" ,        &Engine::gear_ratios) // vector<float> registered in PythonBinding.cpp
		;

	register_ptr_to_python< boost::shared_ptr<RigDef::Engine> >();

	class_<RigDef::Axle>("Axle")
		.def_readonly("OPTION_o_OPEN"   , &Axle::OPTION_o_OPEN  )
		.def_readonly("OPTION_l_LOCKED" , &Axle::OPTION_l_LOCKED)
		.def_readonly("OPTION_s_SPLIT"  , &Axle::OPTION_s_SPLIT )

		.add_property("wheel1_node1", &Axle::PY_GetNode_Wheel1Node1, &Axle::PY_SetNode_Wheel1Node1)
		.add_property("wheel1_node2", &Axle::PY_GetNode_Wheel1Node2, &Axle::PY_SetNode_Wheel1Node2)
		.add_property("wheel2_node1", &Axle::PY_GetNode_Wheel2Node1, &Axle::PY_SetNode_Wheel2Node1)
		.add_property("wheel2_node2", &Axle::PY_GetNode_Wheel2Node2, &Axle::PY_SetNode_Wheel2Node2)

		.def_readwrite("options" , &Axle::options) // vector<char> registered in PythonBinding.cpp
		;

	class_<std::vector<RigDef::Axle> >("AxleVector")
		.def(vector_indexing_suite< std::vector<RigDef::Axle> >())
		;

	enum_<RigDef::Engoption::EngineType>("EngineType")
		.value("TRUCK",           Engoption::ENGINE_TYPE_t_TRUCK)
		.value("CAR",             Engoption::ENGINE_TYPE_c_CAR)
		.value("ELECTRIC_CAR",    Engoption::ENGINE_TYPE_e_ECAR)
		.value("INVALID",         Engoption::ENGINE_TYPE_INVALID)
		;

	class_<RigDef::Engoption>("Engoption")
		.def_readwrite("inertia" ,                   &Engoption::inertia)
		.def_readwrite("type" ,                      &Engoption::type)
		.def_readwrite("clutch_force" ,              &Engoption::clutch_force)
		.def_readwrite("_clutch_force_use_default" , &Engoption::_clutch_force_use_default)
		.def_readwrite("shift_time" ,                &Engoption::shift_time)
		.def_readwrite("clutch_time" ,               &Engoption::clutch_time)
		.def_readwrite("post_shift_time" ,           &Engoption::post_shift_time)
		.def_readwrite("idle_rpm" ,                  &Engoption::idle_rpm)
		.def_readwrite("_idle_rpm_use_default" ,     &Engoption::_idle_rpm_use_default)
		.def_readwrite("stall_rpm" ,                 &Engoption::stall_rpm)
		.def_readwrite("max_idle_mixture" ,          &Engoption::max_idle_mixture)
		.def_readwrite("min_idle_mixture" ,          &Engoption::min_idle_mixture)
		;

	register_ptr_to_python< boost::shared_ptr<RigDef::Engoption> >();

	class_<RigDef::TorqueCurve::Sample>("TorqueCurveSample")
		.def_readwrite("power" ,          &TorqueCurve::Sample::power)
		.def_readwrite("torque_percent" , &TorqueCurve::Sample::torque_percent)
		;

	class_<std::vector<RigDef::TorqueCurve::Sample> >("TorqueCurveSampleVector")
		.def(vector_indexing_suite< std::vector<RigDef::TorqueCurve::Sample> >())
		;

	class_<RigDef::TorqueCurve>("TorqueCurve")
		.def_readwrite("samples" , &TorqueCurve::samples)
		.def_readwrite("predefined_func_name" , &TorqueCurve::predefined_func_name)
		;

	register_ptr_to_python< boost::shared_ptr<RigDef::TorqueCurve> >();

}

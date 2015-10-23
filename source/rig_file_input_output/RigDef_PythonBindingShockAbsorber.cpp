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

void PythonBinding::ExportShockAbsorber()
{
    PYTHON_REGISTER_STD_VECTOR(RigDef::Shock, "ShockAbsorberVector")
    class_<RigDef::Shock>("ShockAbsorber")
        .add_property("node_1", &Shock::PY_GetNode1, &Shock::PY_SetNode1)
        .add_property("node_2", &Shock::PY_GetNode2, &Shock::PY_SetNode2)
        
        .add_property("use_option_i_invisible",    &Shock::HasOption_i_Invisible,    &Shock::SetOption_i_Invisible)
        .add_property("use_option_l_active_left",  &Shock::HasOption_L_ActiveLeft,   &Shock::SetOption_L_ActiveLeft)
        .add_property("use_option_r_active_right", &Shock::HasOption_R_ActiveRight,  &Shock::SetOption_R_ActiveRight)
        .add_property("use_option_m_metric",       &Shock::HasOption_m_Metric,       &Shock::SetOption_m_Metric)
        
        .def_readwrite("spring_rate",    &Shock::spring_rate)
        .def_readwrite("damping",        &Shock::damping)
        .def_readwrite("short_bound",    &Shock::short_bound)
        .def_readwrite("long_bound",     &Shock::long_bound)
        .def_readwrite("precompression", &Shock::precompression)
        .def_readwrite("options",        &Shock::options)
        .def_readwrite("beam_defaults",  &Shock::beam_defaults)
        .def_readwrite("detacher_group", &Shock::detacher_group)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Shock2, "ShockAbsorber2Vector")
    class_<RigDef::Shock2>("ShockAbsorber2")   
        .add_property("node_1", &Shock2::PY_GetNode1, &Shock2::PY_SetNode1)
        .add_property("node_2", &Shock2::PY_GetNode2, &Shock2::PY_SetNode2)
        
        .add_property("option_i_invisible",        &Shock::HasOption_i_Invisible,         &SetOption_i_Invisible) 
        .add_property("option_s_soft_bump_bounds", &Shock::HasOption_s_SoftBumpBounds,    &SetOption_s_SoftBumpBounds)
        .add_property("option_m_metric",           &Shock::HasOption_m_Metric,            &SetOption_m_Metric)
        .add_property("option_m_absolute_metric",  &Shock::HasOption_M_AbsoluteMetric,    &SetOption_M_AbsoluteMetric)
        
        .def_readwrite("spring_in",                  &Shock::spring_in)
        .def_readwrite("damp_in",                    &Shock::damp_in)
        .def_readwrite("progress_factor_spring_in",  &Shock::progress_factor_spring_in)
        .def_readwrite("progress_factor_damp_in",    &Shock::progress_factor_damp_in)
        .def_readwrite("spring_out",                 &Shock::spring_out)
        .def_readwrite("damp_out",                   &Shock::damp_out)
        .def_readwrite("progress_factor_spring_out", &Shock::progress_factor_spring_out)
        .def_readwrite("progress_factor_damp_out",   &Shock::progress_factor_damp_out)
        .def_readwrite("short_bound",                &Shock::short_bound)
        .def_readwrite("long_bound",                 &Shock::long_bound)
        .def_readwrite("precompression",             &Shock::precompression)
        .def_readwrite("beam_defaults",              &Shock::beam_defaults)
        .def_readwrite("detacher_group",             &Shock::detacher_group)
        ;

}

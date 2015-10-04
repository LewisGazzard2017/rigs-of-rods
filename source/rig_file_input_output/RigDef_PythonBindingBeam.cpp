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

void PythonBinding::ExportBeam()
{
	class_<RigDef::BeamDefaultsScale>("BeamDefaultsScale")
		.def_readwrite("springiness"                   , &BeamDefaultsScale::springiness                   )
		.def_readwrite("damping_constant"              , &BeamDefaultsScale::damping_constant              )
		.def_readwrite("deformation_threshold_constant", &BeamDefaultsScale::deformation_threshold_constant)
		.def_readwrite("breaking_threshold_constant"   , &BeamDefaultsScale::breaking_threshold_constant   )
		;

	class_<RigDef::BeamDefaults>("BeamDefaults")
		.def_readwrite("springiness"                   ,  &BeamDefaults::springiness                   )
		.def_readwrite("damping_constant"              ,  &BeamDefaults::damping_constant              )
		.def_readwrite("deformation_threshold_constant",  &BeamDefaults::deformation_threshold_constant)
		.def_readwrite("breaking_threshold_constant"   ,  &BeamDefaults::breaking_threshold_constant   )
		.def_readwrite("visual_beam_diameter",            &BeamDefaults::visual_beam_diameter)
		.def_readwrite("plastic_deformation_coefficient", &BeamDefaults::plastic_deformation_coefficient)
		.def_readwrite("scale",                           &BeamDefaults::scale)
		// Informs whether "enable_advanced_deformation" directive preceded these defaults.
		.def_readwrite("_enable_advanced_deformation",    &BeamDefaults::_enable_advanced_deformation)

		.def("get_scaled_springiness",        &BeamDefaults::GetScaledSpringiness)
		.def("get_scaled_damping",            &BeamDefaults::GetScaledDamping)
		.def("get_scaled_breaking_threshold", &BeamDefaults::GetScaledBreakingThreshold)
		.def("get_scaled_deform_threshold",   &BeamDefaults::GetScaledDeformThreshold)
		;

	register_ptr_to_python< boost::shared_ptr<RigDef::BeamDefaults> >();

	class_<RigDef::Beam>("Beam")
		.add_property("node_1", &Beam::PY_GetNode1, &Beam::PY_SetNode1)
		.add_property("node_2", &Beam::PY_GetNode2, &Beam::PY_SetNode2)

		.add_property("option_i_invisible", &Beam::HasFlag_i_Invisible, &Beam::SetFlag_i_Invisible)
		.add_property("option_r_rope",      &Beam::HasFlag_r_Rope,      &Beam::SetFlag_r_Rope)
		.add_property("option_s_support",   &Beam::HasFlag_s_Support,   &Beam::SetFlag_s_Support)

		.def_readwrite("extension_break_limit"     , &Beam::extension_break_limit)     
		.def_readwrite("_has_extension_break_limit", &Beam::_has_extension_break_limit)
		.def_readwrite("detacher_group"            , &Beam::detacher_group)            
		.def_readwrite("defaults"                  , &Beam::defaults)                  
		;

	class_<std::vector<RigDef::Beam> >("BeamVector")
		.def(vector_indexing_suite< std::vector<RigDef::Beam> >())
		;
}

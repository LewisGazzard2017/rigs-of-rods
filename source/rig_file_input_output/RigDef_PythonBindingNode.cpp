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
	@date   09/2015
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

void PythonBinding::ExportNode()
{
	class_<RigDef::Node::Id>("NodeId")
		.def("to_str", &Node::Id::ToString)

		.add_property("id_str",    &Node::Id::StrCopy, &Node::Id::SetStr)
		.add_property("id_num",    &Node::Id::Num, &Node::Id::SetNum)
		;

	class_<RigDef::Node::Ref>("NodeRef")
		.def("get_id_str", &Node::Ref::StrCopy)
		.def("get_id_num", &Node::Ref::Num)
		;

	class_<RigDef::NodeDefaults>("NodePreset")
		.def_readwrite("load_weight", &NodeDefaults::load_weight)
		.def_readwrite("friction",    &NodeDefaults::friction)
		.def_readwrite("volume",      &NodeDefaults::volume)
		.def_readwrite("surface",     &NodeDefaults::surface)

		.add_property("option_n", &NodeDefaults::HasFlag_n, &NodeDefaults::SetFlag_n)
		.add_property("option_m", &NodeDefaults::HasFlag_m, &NodeDefaults::SetFlag_m)
		.add_property("option_f", &NodeDefaults::HasFlag_f, &NodeDefaults::SetFlag_f)
		.add_property("option_x", &NodeDefaults::HasFlag_x, &NodeDefaults::SetFlag_x)
		.add_property("option_y", &NodeDefaults::HasFlag_y, &NodeDefaults::SetFlag_y)
		.add_property("option_c", &NodeDefaults::HasFlag_c, &NodeDefaults::SetFlag_c)
		.add_property("option_h", &NodeDefaults::HasFlag_h, &NodeDefaults::SetFlag_h)
		.add_property("option_e", &NodeDefaults::HasFlag_e, &NodeDefaults::SetFlag_e)
		.add_property("option_b", &NodeDefaults::HasFlag_b, &NodeDefaults::SetFlag_b)
		.add_property("option_p", &NodeDefaults::HasFlag_p, &NodeDefaults::SetFlag_p)
		.add_property("option_L", &NodeDefaults::HasFlag_L, &NodeDefaults::SetFlag_L)
		.add_property("option_l", &NodeDefaults::HasFlag_l, &NodeDefaults::SetFlag_l)
		;

	class_<RigDef::Node>("Node")
		.def_readwrite("id",                        &Node::id)
		.def_readwrite("position",                  &Node::position)
		.def_readwrite("load_weight_override",      &Node::load_weight_override)
		.def_readwrite("_has_load_weight_override", &Node::_has_load_weight_override)
		.def_readwrite("node_defaults",             &Node::node_defaults)
		.def_readwrite("beam_defaults",             &Node::beam_defaults)
		.def_readwrite("detacher_group",            &Node::detacher_group)

		.add_property("option_n", &Node::HasFlag_n, &Node::SetFlag_n)
		.add_property("option_m", &Node::HasFlag_m, &Node::SetFlag_m)
		.add_property("option_f", &Node::HasFlag_f, &Node::SetFlag_f)
		.add_property("option_x", &Node::HasFlag_x, &Node::SetFlag_x)
		.add_property("option_y", &Node::HasFlag_y, &Node::SetFlag_y)
		.add_property("option_c", &Node::HasFlag_c, &Node::SetFlag_c)
		.add_property("option_h", &Node::HasFlag_h, &Node::SetFlag_h)
		.add_property("option_e", &Node::HasFlag_e, &Node::SetFlag_e)
		.add_property("option_b", &Node::HasFlag_b, &Node::SetFlag_b)
		.add_property("option_p", &Node::HasFlag_p, &Node::SetFlag_p)
		.add_property("option_L", &Node::HasFlag_L, &Node::SetFlag_L)
		.add_property("option_l", &Node::HasFlag_l, &Node::SetFlag_l)
		;

	class_<std::vector<RigDef::Node> >("NodeVector")
		.def(vector_indexing_suite< std::vector<RigDef::Node> >())
		;

	class_<RigDef::NodeGroupWithPreset>("NodeGroupWithPreset")
		.def_readwrite("preset",  &NodeGroupWithPreset::preset)
		.def_readwrite("nodes",   &NodeGroupWithPreset::nodes)
		;

	class_<std::vector<RigDef::NodeGroupWithPreset> >("NodeGroupVector")
		.def(vector_indexing_suite< std::vector<RigDef::NodeGroupWithPreset> >())
		;

}

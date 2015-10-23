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

void PythonBinding::ExportFile()
{
	PYTHON_REGISTER_SHARED_PTR(RigDef::File)
	class_<RigDef::File>("File")
		.def_readonly("description",                    &File::PY_GetDescription)

		.def_readwrite("file_format_version",           &File::file_format_version)
		.def_readwrite("guid",                          &File::guid)
		.def_readwrite("hide_in_chooser",               &File::hide_in_chooser)
		.def_readwrite("enable_advanced_deformation",   &File::enable_advanced_deformation)
		.def_readwrite("slide_nodes_connect_instantly", &File::slide_nodes_connect_instantly)
		.def_readwrite("rollon",                        &File::rollon)
		.def_readwrite("forward_commands",              &File::forward_commands)
		.def_readwrite("import_commands",               &File::import_commands)
		.def_readwrite("lockgroup_default_nolock",      &File::lockgroup_default_nolock)
		.def_readwrite("rescuer",                       &File::rescuer)
		.def_readwrite("disable_default_sounds",        &File::disable_default_sounds)
		.def_readwrite("name",                          &File::name)
		.def_readwrite("collision_range",               &File::collision_range)
		.def_readwrite("_collision_range_set",          &File::_collision_range_set)
		.def_readwrite("minimum_mass",                  &File::minimum_mass)
		.def_readwrite("_minimum_mass_set",             &File::_minimum_mass_set)
		.def_readwrite("authors",                       &File::authors)
		.def_readwrite("file_info",                     &File::file_info)
		.def_readwrite("root_module",                   &File::root_module)
		;

	PYTHON_REGISTER_SHARED_PTR(RigDef::File::Module)
	class_<RigDef::File::Module>("Module")
		.def_readwrite("name",                      &File::Module::name)
		.def_readwrite("globals",                   &File::Module::globals)

		// Structure
		.def_readwrite("nodes_by_preset",           &File::Module::nodes_by_preset)
		.def_readwrite("beams_by_preset",           &File::Module::beams_by_preset)
		.def_readwrite("shocks_by_preset",          &File::Module::shocks_by_preset)
		.def_readwrite("shocks2_by_preset",         &File::Module::shocks2_by_preset)
		.def_readwrite("command_hydros_by_preset",  &File::Module::commands2_by_preset)
		.def_readwrite("steering_hydros_by_preset", &File::Module::hydros_by_preset)
		.def_readwrite("ropes_by_preset",           &File::Module::ropes_by_preset)

		// Powertrain
		.def_readwrite("engine",                    &File::Module::engine)
		.def_readwrite("engoption",                 &File::Module::engoption)
		.def_readwrite("torque_curve",              &File::Module::torque_curve)
		.def_readwrite("axles",                     &File::Module::axles)
		// Aerial
		.def_readwrite("turboprops",                &File::Module::turboprops_2)
		.def_readwrite("turbojets",                 &File::Module::turbojets)
		.def_readwrite("pistonprops",               &File::Module::pistonprops)
		.def_readwrite("wings",                     &File::Module::wings)
		.def_readwrite("airbrakes",                 &File::Module::airbrakes)
		.def_readwrite("fusedrag",                  &File::Module::fusedrag)
		// Wheels
		.def_readwrite("wheels",                    &File::Module::wheels)
		.def_readwrite("wheels_2",                  &File::Module::wheels_2)
		.def_readwrite("mesh_wheels",               &File::Module::mesh_wheels)
		.def_readwrite("mesh_wheels_2",             &File::Module::mesh_wheels_2)
		.def_readwrite("flex_body_wheels",          &File::Module::flex_body_wheels)
		;

	PYTHON_REGISTER_STD_VECTOR(RigDef::Author, "AuthorVector")
	class_<RigDef::Author>("Author")
		.def_readwrite("type",               &Author::type)
		.def_readwrite("forum_account_id",   &Author::forum_account_id)
		.def_readwrite("name",               &Author::name)
		.def_readwrite("email",              &Author::email)
		.def_readwrite("_has_forum_account", &Author::_has_forum_account)
		;

	class_<RigDef::Globals>("Globals")
		.def_readwrite("dry_mass",      &Globals::dry_mass)
		.def_readwrite("cargo_mass",    &Globals::cargo_mass)
		.def_readwrite("material_name", &Globals::material_name)
		;

	PYTHON_REGISTER_SHARED_PTR(RigDef::Fileinfo)
	class_<RigDef::Fileinfo>("FileInfo")
		.def_readwrite("unique_id",             &Fileinfo::unique_id)
		.def_readwrite("category_id",           &Fileinfo::category_id)
		.def_readwrite("file_version",          &Fileinfo::file_version)
		.def_readwrite("_has_unique_id",        &Fileinfo::_has_unique_id)
		.def_readwrite("_has_category_id",      &Fileinfo::_has_category_id)
		.def_readwrite("_has_file_version_set", &Fileinfo::_has_file_version_set)
		;

};

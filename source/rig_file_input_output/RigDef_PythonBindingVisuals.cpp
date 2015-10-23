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

void PythonBinding::ExportVisuals()
{

    enum_<RigDef::ManagedMaterial::Type>("ManagedMaterialType")
        .value("FLEXMESH_STANDARD",    ManagedMaterial::TYPE_FLEXMESH_STANDARD)
        .value("FLEXMESH_TRANSPARENT", ManagedMaterial::TYPE_FLEXMESH_TRANSPARENT)
        .value("MESH_STANDARD",        ManagedMaterial::TYPE_MESH_STANDARD)
        .value("MESH_TRANSPARENT",     ManagedMaterial::TYPE_MESH_TRANSPARENT)
        .value("INVALID",              ManagedMaterial::TYPE_INVALID)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::ManagedMaterial, "ManagedMaterialVector")
    class_<RigDef::ManagedMaterial>("ManagedMaterial")
        .def_readwrite("type",                   &ManagedMaterial::type)
        .def_readwrite("diffuse_map",            &ManagedMaterial::diffuse_map)
        .def_readwrite("damaged_diffuse_map",    &ManagedMaterial::damaged_diffuse_map)
        .def_readwrite("specular_map",           &ManagedMaterial::specular_map)

        .def("has_damaged_diffuse_map",          &ManagedMaterial::HasDamagedDiffuseMap)
        .def("has_specular_map",                 &ManagedMaterial::HasSpecularMap)

		.add_property("use_option_doublesided", &ManagedMaterial::HasOption_DoubleSided, &ManagedMaterial::SetOption_DoubleSided)
        ;

    PYTHON_REGISTER_SHARED_PTR(RigDef::SkeletonSettings)
    class_<RigDef::SkeletonSettings>("SkeletonSettings")
        .def_readwrite("visibility_range_meters", &SkeletonSettings::visibility_range_meters)
        .def_readwrite("beam_thickness_meters",   &SkeletonSettings::beam_thickness_meters)
        ;
    
    PYTHON_REGISTER_STD_VECTOR(RigDef::Flexbody, "FlexbodyVector")
    class_<RigDef::Flexbody>("Flexbody")
        .def_readwrite("reference_node",   &Flexbody::reference_node)
        .def_readwrite("x_axis_node",      &Flexbody::x_axis_node)
        .def_readwrite("y_axis_node",      &Flexbody::y_axis_node)
        .def_readwrite("offset",           &Flexbody::offset)
        .def_readwrite("rotation",         &Flexbody::rotation)
        .def_readwrite("mesh_name",        &Flexbody::mesh_name)
        .def_readwrite("animations",       &Flexbody::animations)
        .def_readwrite("node_list",        &Flexbody::node_list)
        .def_readwrite("camera_settings",  &Flexbody::camera_settings)
        ;

    enum_<RigDef::GuiSettings::MapMode>("MapMode")
        .value("OFF",       GuiSettings::MAP_MODE_OFF)
        .value("SIMPLE",    GuiSettings::MAP_MODE_SIMPLE)
        .value("ZOOM",      GuiSettings::MAP_MODE_ZOOM)
        .value("INVALID",   GuiSettings::MAP_MODE_INVALID)
        ; 

    PYTHON_REGISTER_SHARED_PTR(RigDef::GuiSettings)
    class_<RigDef::GuiSettings>("GuiSettings")   
        .def_readwrite("tacho_material",                &GuiSettings::tacho_material)
        .def_readwrite("speedo_material",               &GuiSettings::speedo_material)
        .def_readwrite("speedo_highest_kph",            &GuiSettings::speedo_highest_kph)
        .def_readwrite("use_max_rpm",                   &GuiSettings::use_max_rpm)
        .def_readwrite("help_material",                 &GuiSettings::help_material)
        .def_readwrite("interactive_overview_map_mode", &GuiSettings::interactive_overview_map_mode)
        .def_readwrite("dashboard_layouts",             &GuiSettings::dashboard_layouts)
        .def_readwrite("rtt_dashboard_layouts",         &GuiSettings::rtt_dashboard_layouts)
        ;
}

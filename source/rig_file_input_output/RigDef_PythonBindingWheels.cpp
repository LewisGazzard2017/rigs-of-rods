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

#define REGISTER_WHEEL_NODE_ATTRS(CLASSNAME)                                  \
    .add_property("node_1", &CLASSNAME::PY_GetNode1, &CLASSNAME::PY_SetNode1) \
    .add_property("node_2", &CLASSNAME::PY_GetNode2, &CLASSNAME::PY_SetNode2) \

#define REGISTER_BASE_WHEEL_ATTRS(CLASSNAME)                             \
    REGISTER_WHEEL_NODE_ATTRS(CLASSNAME)                                 \
    .def_readwrite("width",              &CLASSNAME::width)              \
    .def_readwrite("num_rays",           &CLASSNAME::num_rays)           \
    .def_readwrite("rigidity_node",      &CLASSNAME::rigidity_node)      \
    .def_readwrite("braking",            &CLASSNAME::braking)            \
    .def_readwrite("propulsion",         &CLASSNAME::propulsion)         \
    .def_readwrite("reference_arm_node", &CLASSNAME::reference_arm_node) \
    .def_readwrite("mass",               &CLASSNAME::mass)               \
    .def_readwrite("node_preset",        &CLASSNAME::node_defaults)      \
    .def_readwrite("beam_preset",        &CLASSNAME::beam_defaults)      \

#define REGISTER_BASE_WHEEL_2_ATTRS(CLASSNAME)                          \
    REGISTER_WHEEL_NODE_ATTRS(CLASSNAME)                                \
    .def_readwrite("rim_radius",       &CLASSNAME::rim_radius)          \
    .def_readwrite("tyre_radius",      &CLASSNAME::tyre_radius)         \
    .def_readwrite("tyre_springiness", &CLASSNAME::tyre_springiness)    \
    .def_readwrite("tyre_damping",     &CLASSNAME::tyre_damping)        \
      

void PythonBinding::ExportWheels()
{

    enum_<RigDef::Wheels::Braking>("WheelBraking")
        .value("NO",                Wheels::BRAKING_NO)
        .value("YES",               Wheels::BRAKING_YES)
        .value("DIRECTIONAL_LEFT",  Wheels::BRAKING_DIRECTIONAL_LEFT)
        .value("DIRECTIONAL_RIGHT", Wheels::BRAKING_DIRECTIONAL_RIGHT)
        .value("ONLY_FOOT",         Wheels::BRAKING_ONLY_FOOT)
        .value("INVALID",           Wheels::BRAKING_INVALID)
        ;

    enum_<RigDef::Wheels::Propulsion>("WheelPropulsion")
        .value("NONE",     Wheels::PROPULSION_NONE)
        .value("FORWARD",  Wheels::PROPULSION_FORWARD)
        .value("BACKWARD", Wheels::PROPULSION_BACKWARD)
        .value("INVALID",  Wheels::PROPULSION_INVALID)
        ;
        
    class_<RigDef::Wheel>("Wheel")
        REGISTER_BASE_WHEEL_ATTRS(Wheel)
        .def_readwrite("radius",             &Wheel::radius)
        .def_readwrite("springiness",        &Wheel::springiness)
        .def_readwrite("damping",            &Wheel::damping)
        .def_readwrite("face_material_name", &Wheel::face_material_name)
        .def_readwrite("band_material_name", &Wheel::band_material_name)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Wheel, "WheelVector")
    
    class_<RigDef::Wheel2>("Wheel2")
        REGISTER_BASE_WHEEL_2_ATTRS(Wheel2)
        .def_readwrite("face_material_name", &Wheel2::face_material_name)
        .def_readwrite("band_material_name", &Wheel2::band_material_name)
        .def_readwrite("rim_springiness",    &Wheel2::rim_springiness)
        .def_readwrite("rim_damping",        &Wheel2::rim_damping)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Wheel2, "Wheel2Vector")

    enum_<RigDef::MeshWheel::Side>("MeshWheelSide")
        .value("SIDE_INVALID",   MeshWheel::SIDE_INVALID)
        .value("SIDE_RIGHT",     MeshWheel::SIDE_RIGHT)
        .value("SIDE_LEFT",      MeshWheel::SIDE_LEFT)
        ;

    class_<RigDef::MeshWheel>("MeshWheel")
        REGISTER_BASE_WHEEL_ATTRS(MeshWheel)
        .def_readwrite("side",          &MeshWheel::side)
        .def_readwrite("mesh_name",     &MeshWheel::mesh_name)
        .def_readwrite("material_name", &MeshWheel::material_name)
        .def_readwrite("rim_radius",    &MeshWheel::rim_radius)
        .def_readwrite("tyre_radius",   &MeshWheel::tyre_radius)
        .def_readwrite("spring",        &MeshWheel::spring)
        .def_readwrite("damping",       &MeshWheel::damping)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::MeshWheel, "MeshWheelVector")
        
    class_<RigDef::MeshWheel2>("MeshWheel2")
        REGISTER_BASE_WHEEL_2_ATTRS(MeshWheel2)        
        .def_readwrite("side",          &MeshWheel2::side)
        .def_readwrite("mesh_name",     &MeshWheel2::mesh_name)
        .def_readwrite("material_name", &MeshWheel2::material_name)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::MeshWheel2, "MeshWheel2Vector")
        
    class_<RigDef::FlexBodyWheel>("FlexBodyWheel")
        REGISTER_BASE_WHEEL_2_ATTRS(FlexBodyWheel)        
        .def_readwrite("side",             &FlexBodyWheel::side)
        .def_readwrite("rim_springiness",  &FlexBodyWheel::rim_springiness)
        .def_readwrite("rim_damping",      &FlexBodyWheel::rim_damping)
        .def_readwrite("rim_mesh_name",    &FlexBodyWheel::rim_mesh_name)   
        .def_readwrite("tyre_mesh_name",   &FlexBodyWheel::tyre_mesh_name)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::FlexBodyWheel, "FlexBodyWheelVector")

}

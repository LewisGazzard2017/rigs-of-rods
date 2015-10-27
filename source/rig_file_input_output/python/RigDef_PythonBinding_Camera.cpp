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

void PythonBinding::ExportCamera()
{
    PYTHON_REGISTER_STD_VECTOR(RigDef::VideoCamera, "VideoCameraVector")
    class_<RigDef::VideoCamera>("VideoCamera")
        .def_readwrite("offset",               &VideoCamera::offset)
        .def_readwrite("rotation",             &VideoCamera::rotation)
        .def_readwrite("left_node",            &VideoCamera::left_node)
        .def_readwrite("camera_name",          &VideoCamera::camera_name)
        .def_readwrite("camera_role",          &VideoCamera::camera_role)
        .def_readwrite("camera_mode",          &VideoCamera::camera_mode)
        .def_readwrite("bottom_node",          &VideoCamera::bottom_node)
        .def_readwrite("field_of_view",        &VideoCamera::field_of_view)
        .def_readwrite("texture_width",        &VideoCamera::texture_width)
        .def_readwrite("material_name",        &VideoCamera::material_name)
        .def_readwrite("texture_height",       &VideoCamera::texture_height)
        .def_readwrite("reference_node",       &VideoCamera::reference_node)
        .def_readwrite("min_clip_distance",    &VideoCamera::min_clip_distance)
        .def_readwrite("max_clip_distance",    &VideoCamera::max_clip_distance)
        .def_readwrite("alt_reference_node",   &VideoCamera::alt_reference_node)
        .def_readwrite("alt_orientation_node", &VideoCamera::alt_orientation_node)
        ;

    enum_<RigDef::CameraSettings::Mode>("CameraSettingsMode")
        .value("ALWAYS",      RigDef::CameraSettings::MODE_ALWAYS  )
        .value("EXTERNAL",    RigDef::CameraSettings::MODE_EXTERNAL)
        .value("CINECAM",     RigDef::CameraSettings::MODE_CINECAM )
        .value("INVALID",     RigDef::CameraSettings::MODE_INVALID )
        ;

    class_<RigDef::CameraSettings>("CameraSettings")   
        .def_readwrite("mode",          &CameraSettings::mode)
        .def_readwrite("cinecam_index", &CameraSettings::cinecam_index)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Camera, "CameraVector")
    class_<RigDef::Camera>("Camera")   
        .def_readwrite("center_node", &Camera::center_node)
        .def_readwrite("back_node",   &Camera::back_node)
        .def_readwrite("left_node",   &Camera::left_node)
        ;

    class_<RigDef::CameraRail>("CameraRail")   
        .def_readwrite("nodes", &CameraRail::nodes)
        ;
    
    PYTHON_REGISTER_STD_VECTOR(RigDef::Cinecam, "CinecamVector")
    class_<RigDef::Cinecam>("Cinecam")   
        .def_readwrite("position",      &Cinecam::position)
        .def_readwrite("spring",        &Cinecam::spring)
        .def_readwrite("damping",       &Cinecam::damping)
        .def_readwrite("beam_defaults", &Cinecam::beam_defaults)
        .def_readwrite("node_defaults", &Cinecam::node_defaults)
        
        .def("get_node", &Cinecam::PY_GetNode)
        .def("set_node", &Cinecam::PY_SetNode)
        ;

    enum_<RigDef::ExtCamera::Mode>("ExtCameraMode")  
        .value("MODE_CLASSIC",  ExtCamera::MODE_CLASSIC)
        .value("MODE_CINECAM",  ExtCamera::MODE_CINECAM)
        .value("MODE_NODE",     ExtCamera::MODE_NODE   )
        .value("MODE_INVALID",  ExtCamera::MODE_INVALID)
        ;

    PYTHON_REGISTER_SHARED_PTR(ExtCamera)
    class_<RigDef::ExtCamera>("ExtCamera")   
        .def_readwrite("mode", &ExtCamera::mode)
        .def_readwrite("node", &ExtCamera::node)
        ;
}

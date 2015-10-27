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

static boost::python::object PY_Wing_GetTexcoord(boost::python::object _py_this, int index)
{
	Wing* _this = extract<Wing*>(_py_this);
	boost::python::object o;
	o.attr("X") = _this->tex_coords[index*2];
	o.attr("Y") = _this->tex_coords[(index*2)+1];
	return o;
}

void PythonBinding::ExportAerial()
{
    enum_<RigDef::Wing::Control>("WingControlSurface")
        .value("n_NONE",                   Wing::CONTROL_n_NONE)
        .value("a_RIGHT_AILERON",          Wing::CONTROL_a_RIGHT_AILERON)
        .value("b_LEFT_AILERON",           Wing::CONTROL_b_LEFT_AILERON)
        .value("f_FLAP",                   Wing::CONTROL_f_FLAP)
        .value("e_ELEVATOR",               Wing::CONTROL_e_ELEVATOR)
        .value("r_RUDDER",                 Wing::CONTROL_r_RUDDER)
        .value("S_RIGHT_HAND_STABILATOR",  Wing::CONTROL_S_RIGHT_HAND_STABILATOR)
        .value("T_LEFT_HAND_STABILATOR",   Wing::CONTROL_T_LEFT_HAND_STABILATOR)
        .value("c_RIGHT_ELEVON",           Wing::CONTROL_c_RIGHT_ELEVON)
        .value("d_LEFT_ELEVON",            Wing::CONTROL_d_LEFT_ELEVON)
        .value("g_RIGHT_FLAPERON",         Wing::CONTROL_g_RIGHT_FLAPERON)
        .value("h_LEFT_FLAPERON",          Wing::CONTROL_h_LEFT_FLAPERON)
        .value("U_RIGHT_HAND_TAILERON",    Wing::CONTROL_U_RIGHT_HAND_TAILERON)
        .value("V_LEFT_HAND_TAILERON",     Wing::CONTROL_V_LEFT_HAND_TAILERON)
        .value("i_RIGHT_RUDDERVATOR",      Wing::CONTROL_i_RIGHT_RUDDERVATOR)
        .value("j_LEFT_RUDDERVATOR",       Wing::CONTROL_j_LEFT_RUDDERVATOR)
        .value("INVALID",                  Wing::CONTROL_INVALID)
        ;
    
    class_<RigDef::Wing>("Wing")
        .def_readwrite("control_surface", &Wing::control_surface)
        .def_readwrite("chord_point",     &Wing::chord_point)
        .def_readwrite("min_deflection",  &Wing::min_deflection)
        .def_readwrite("max_deflection",  &Wing::max_deflection)
        .def_readwrite("airfoil_name",    &Wing::airfoil)
        .def_readwrite("efficacy_coef",   &Wing::efficacy_coef)
        
        .def("get_node",        &Wing::PY_GetNode)
        .def("set_node",        &Wing::PY_SetNode)
        .def("get_texcoord",    PY_Wing_GetTexcoord)
        .def("set_texcoord",    &Wing::PY_SetTexcoord)
        ;

    class_<std::vector<RigDef::Wing> >("WingVector")
         .def(vector_indexing_suite< std::vector<RigDef::Wing> >())
         ;
        
    class_<RigDef::Airbrake>("Airbrake")    
        .def_readwrite("reference_node",        &Airbrake::reference_node)
        .def_readwrite("x_axis_node",           &Airbrake::x_axis_node)
        .def_readwrite("y_axis_node",           &Airbrake::y_axis_node)
        .def_readwrite("aditional_node",        &Airbrake::aditional_node)
        .def_readwrite("offset",                &Airbrake::offset)
        .def_readwrite("width",                 &Airbrake::width)
        .def_readwrite("height",                &Airbrake::height)
        .def_readwrite("max_inclination_angle", &Airbrake::max_inclination_angle)
        .def_readwrite("texcoord_x1",           &Airbrake::texcoord_x1)
        .def_readwrite("texcoord_x2",           &Airbrake::texcoord_x2)
        .def_readwrite("texcoord_y1",           &Airbrake::texcoord_y1)
        .def_readwrite("texcoord_y2",           &Airbrake::texcoord_y2)
        .def_readwrite("lift_coefficient",      &Airbrake::lift_coefficient)
        ;
        

    class_<std::vector<RigDef::Airbrake> >("AirbrakeVector")
         .def(vector_indexing_suite< std::vector<RigDef::Airbrake> >())
         ;

    class_<RigDef::Turbojet>("Turbojet")   
        .def_readwrite("front_node",     &Turbojet::front_node)
        .def_readwrite("back_node",      &Turbojet::back_node)
        .def_readwrite("side_node",      &Turbojet::side_node)
        .def_readwrite("is_reversable",  &Turbojet::is_reversable)
        .def_readwrite("dry_thrust",     &Turbojet::dry_thrust)
        .def_readwrite("wet_thrust",     &Turbojet::wet_thrust)
        .def_readwrite("front_diameter", &Turbojet::front_diameter)
        .def_readwrite("back_diameter",  &Turbojet::back_diameter)
        .def_readwrite("nozzle_length",  &Turbojet::nozzle_length)
        ;

    class_<std::vector<RigDef::Turbojet> >("TurbojetVector")
         .def(vector_indexing_suite< std::vector<RigDef::Turbojet> >())
         ;

    class_<RigDef::Turboprop2>("Turboprop")
        .def("get_blade_tip_node",         &Turboprop2::PY_GetBladeTipNode)
        .def("set_blade_tip_node",         &Turboprop2::PY_SetBladeTipNode)

        .def_readwrite("reference_node",   &Turboprop2::reference_node)
        .def_readwrite("axis_node",        &Turboprop2::axis_node)
        .def_readwrite("turbine_power_kW", &Turboprop2::turbine_power_kW)
        .def_readwrite("airfoil",          &Turboprop2::airfoil)
        .def_readwrite("couple_node",      &Turboprop2::couple_node)
        .def_readwrite("_format_version",  &Turboprop2::_format_version)
        ;

    class_<std::vector<RigDef::Turboprop2> >("TurbopropVector")
         .def(vector_indexing_suite< std::vector<RigDef::Turboprop2> >())
         ;

    class_<RigDef::Pistonprop>("Pistonprop")
        .def("get_blade_tip_node",          &Pistonprop::PY_GetBladeTipNode)
        .def("set_blade_tip_node",          &Pistonprop::PY_SetBladeTipNode)

        .def_readwrite("reference_node",    &Pistonprop::reference_node)
        .def_readwrite("axis_node",         &Pistonprop::axis_node)
        .def_readwrite("couple_node",       &Pistonprop::couple_node)
        .def_readwrite("_couple_node_set",  &Pistonprop::_couple_node_set)
        .def_readwrite("turbine_power_kW",  &Pistonprop::turbine_power_kW)
        .def_readwrite("pitch",             &Pistonprop::pitch)
        .def_readwrite("airfoil",           &Pistonprop::airfoil)
        ;
    
    class_<std::vector<RigDef::Pistonprop> >("PistonpropVector")
         .def(vector_indexing_suite< std::vector<RigDef::Pistonprop> >())
         ;

    class_<RigDef::Fusedrag>("Fusedrag")
        .def_readwrite("use_autocalc",        &Fusedrag::autocalc)
        .def_readwrite("front_node",          &Fusedrag::front_node)
        .def_readwrite("rear_node",           &Fusedrag::rear_node)
        .def_readwrite("approximate_width",   &Fusedrag::approximate_width)
        .def_readwrite("airfoil_name",        &Fusedrag::airfoil_name)
        .def_readwrite("area_coefficient",    &Fusedrag::area_coefficient)
        ;

	register_ptr_to_python< boost::shared_ptr<RigDef::Fusedrag> >();

}

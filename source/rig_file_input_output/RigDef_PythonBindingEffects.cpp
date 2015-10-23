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

void PythonBinding::ExportEffects()
{
    enum_<RigDef::Flare2::Type>("FlareType")
        .value("f_HEADLIGHT",      Flare2::TYPE_f_HEADLIGHT    )
        .value("b_BRAKELIGHT",     Flare2::TYPE_b_BRAKELIGHT   )
        .value("l_LEFT_BLINKER",   Flare2::TYPE_l_LEFT_BLINKER )
        .value("r_RIGHT_BLINKER",  Flare2::TYPE_r_RIGHT_BLINKER)
        .value("R_REVERSE_LIGHT",  Flare2::TYPE_R_REVERSE_LIGHT)
        .value("u_USER",           Flare2::TYPE_u_USER         )
        .value("INVALID",          Flare2::TYPE_INVALID        )
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Flare2, "FlareVector")
    class_<RigDef::Flare2>("Flare")
        .def_readwrite("reference_node",    &Flare2::reference_node)
        .def_readwrite("node_axis_x",       &Flare2::node_axis_x)
        .def_readwrite("node_axis_y",       &Flare2::node_axis_y)
        .def_readwrite("offset",            &Flare2::offset)
        .def_readwrite("type",              &Flare2::type)
        .def_readwrite("control_number",    &Flare2::control_number)
        .def_readwrite("blink_delay_milis", &Flare2::blink_delay_milis)
        .def_readwrite("size",              &Flare2::size)
        .def_readwrite("material_name",     &Flare2::material_name)
        .def_readwrite("material_bindings", &Flare2::material_bindings)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Exhaust, "ExhaustVector")
    class_<RigDef::Exhaust>("Exhaust")   
        .def_readwrite("reference_node", &Exhaust::reference_node)
        .def_readwrite("direction_node", &Exhaust::direction_node)
        .def_readwrite("material_name",  &Exhaust::material_name)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::Particle, "ParticleVector")
    class_<RigDef::Particle>("Particle")   
        .def_readwrite("emitter_node",         &Particle::emitter_node)
        .def_readwrite("reference_node",       &Particle::reference_node)
        .def_readwrite("particle_system_name", &Particle::particle_system_name)
        ;

    enum_<RigDef::SoundSource2::Mode>("SoundSourceMode")
        .value("ALWAYS", SoundSource2::MODE_ALWAYS )
        .value("OUTSIDE", SoundSource2::MODE_OUTSIDE )
        .value("CINECAM", SoundSource2::MODE_CINECAM )
        .value("INVALID", SoundSource2::MODE_INVALID )
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::SoundSource, "SoundSourceVector")
    class_<RigDef::SoundSource>("SoundSource")   
        .def_readwrite("name",              &SoundSource::node)
        .def_readwrite("sound_script_name", &SoundSource::sound_script_name)
        ;

    PYTHON_REGISTER_STD_VECTOR(RigDef::SoundSource2, "SoundSource2Vector")
    class_<RigDef::SoundSource2>("SoundSource2")
        .def_readwrite("name",              &SoundSource2::node)
        .def_readwrite("sound_script_name", &SoundSource2::sound_script_name)
        .def_readwrite("mode",              &SoundSource2::mode)
        .def_readwrite("cinecam_index",     &SoundSource2::cinecam_index)
        ;

}

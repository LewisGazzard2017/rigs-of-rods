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
	@date   09/2015
	@author Petr Ohlidal
*/

#include "RigDef_PythonBinding.h"

#include <boost/python/detail/wrap_python.hpp>
	// Replaces #include <Python.h>, recommended in Boost docs
	// Must be included before any OS heders - Python limitation

#include "RigDef_File.h"
#include "RigDef_Node.h"
#include "RigDef_Parser.h"
#include "RigDef_Validator.h"

#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/register_ptr_to_python.hpp>
#include <boost/python.hpp>

#include <OgreDataStream.h>
#include <OgreResourceGroupManager.h>

using namespace RigDef;

class ParserWrapper
{
public:
	void PY_ParseFile(std::string directory, std::string filename)
	{
		// LOAD \\

		Ogre::DataStreamPtr stream = Ogre::DataStreamPtr();
		Ogre::String resource_group_name("RigEditor_CurrentProject");

		try
		{
			Ogre::ResourceGroupManager::getSingleton().addResourceLocation(directory, "FileSystem", resource_group_name);
			stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename, resource_group_name);
		} 
		catch (Ogre::Exception& e)
		{
			// TODO: Report error to user
			std::stringstream msg;
			msg << "RigEditor: Failed to retrieve rig file [" << filename << "], Ogre::Exception was thrown with message: " << e.what();
			PyErr_SetString(PyExc_RuntimeError, msg.str().c_str());
			return;
		}

		// PARSE \\

		m_parser.Prepare();
		while(! stream->eof())
		{
			m_parser.ParseLine(stream->getLine());
		}
		m_parser.Finalize();

		// VALIDATE \\

		m_validator.Setup(m_parser.GetFile());
		m_valid = m_validator.Validate();

	}

	boost::shared_ptr<File> PY_GetParsedFile()
	{
		return m_validator.GetFileOwnership();
	}

private:
	Parser     m_parser;
	Validator  m_validator;
	bool       m_valid;
};

using namespace boost::python;


BOOST_PYTHON_MODULE(ror_truckfile)
{
	class_<Ogre::Vector3>("Vector3")
		.def_readwrite("x", &Ogre::Vector3::x)
		.def_readwrite("y", &Ogre::Vector3::y)
		.def_readwrite("z", &Ogre::Vector3::z)
		;

	class_<RigDef::Node::Id>("NodeId")
		.def("to_str", &Node::Id::ToString)

		//.add_property("id_str",    &Node::Id::Str, &Node::Id::SetStr)
		.add_property("id_num",    &Node::Id::Num, &Node::Id::SetNum)
		;

	class_<RigDef::NodeDefaults>("NodeDefaults")
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

	class_<RigDef::File::Module>("Module")
		.def_readwrite("nodes", &File::Module::nodes)
		.def_readwrite("name",  &File::Module::name)
		;

	register_ptr_to_python< boost::shared_ptr<RigDef::File::Module> >();

	class_<RigDef::File>("File")
		.def_readwrite("root_module", &File::root_module)
		.def_readwrite("name",        &File::name)
		;

	register_ptr_to_python< boost::shared_ptr<RigDef::File> >();
	
	class_<ParserWrapper>("Parser")
		.def("parse_file",      &ParserWrapper::PY_ParseFile)
		.def("get_parsed_file", &ParserWrapper::PY_GetParsedFile)
		;
}

// Static
void PythonBinding::AppendInittab()
{
	PyImport_AppendInittab("ror_truckfile", PyInit_ror_truckfile); // Function "PyInit_*" defined by BOOST_PYTHON_MODULE
}


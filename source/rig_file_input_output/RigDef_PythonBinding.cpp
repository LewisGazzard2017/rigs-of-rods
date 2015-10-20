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
		auto file = m_validator.GetFileOwnership();
		file->GroupNodesByPreset();
		file->GroupAllBeamTypesByPreset();
		return file;
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

	class_<std::vector<float> >("FloatVector")
		.def(vector_indexing_suite< std::vector<float> >())
		;

	class_<std::vector<char> >("CharVector")
		.def(vector_indexing_suite< std::vector<char> >())
		;

	PythonBinding::ExportAerial();
	PythonBinding::ExportBeam();
	PythonBinding::ExportCommandHydro();
	PythonBinding::ExportFile();
	PythonBinding::ExportNode();
	PythonBinding::ExportPowertrain();
	PythonBinding::ExportWheels();
	
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


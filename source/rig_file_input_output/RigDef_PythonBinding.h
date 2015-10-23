
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
*/

#pragma once

// Assumes boost::python is included and namespace is used.
#define PYTHON_REGISTER_STD_VECTOR(CLASS, PYNAME)                \
	class_<std::vector<CLASS> >(PYNAME)                          \
		.def(vector_indexing_suite< std::vector<CLASS> >())      \
		;

// Assumes boost::python is included and namespace is used.
#define PYTHON_REGISTER_SHARED_PTR(CLASS)                 \
    register_ptr_to_python< boost::shared_ptr<CLASS> >(); \

namespace RigDef
{
	class PythonBinding
	{
		public:
		static void AppendInittab();

		static void ExportNode();
		static void ExportBeam();
		static void ExportPowertrain();
		static void ExportCommandHydro();
		static void ExportAerial();
		static void ExportFile();
		static void ExportWheels();
        static void ExportTrigger();
	};
}


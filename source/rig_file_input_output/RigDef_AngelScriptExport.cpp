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
	@author Petr Ohlidal
	@date   04/2015
*/

#include "RigDef_AngelScriptExport.h"

#include "AngelScriptSetupHelper.h"
#include "RigDef_File.h"
#include "RigDef_Node.h"

using namespace AngelScript;
using namespace RigDef;
using namespace RoR;

INHERIT_STD_SHARED_PTR(AS_RigDef_ModuleSharedPtr, RigDef::File::Module);
INHERIT_STD_SHARED_PTR(AS_RigDef_FileSharedPtr,   RigDef::File);

INHERIT_STD_VECTOR(AS_RigDef_NodeArray,            RigDef::Node);
INHERIT_STD_VECTOR(AS_RigDef_BeamArray,            RigDef::Node);
INHERIT_STD_VECTOR(AS_RigDef_ModuleSharedPtrArray, AS_RigDef_ModuleSharedPtr);

void ExportToAngelScript(AngelScriptSetupHelper* A)
{
	// ##### RECORDS #####

	// Classes
	A->RegisterObjectType("RigDef_Node", sizeof(RigDef::Node), asOBJ_VALUE );
	A->RegisterObjectType("RigDef_Beam", sizeof(RigDef::Beam), asOBJ_VALUE );
	// More to come...

	// STL vectors
	A->RegisterInheritedStdVector<AS_RigDef_NodeArray>("RigDef_NodeArray", "RigDef_Node");
	A->RegisterInheritedStdVector<AS_RigDef_BeamArray>("RigDef_BeamArray", "RigDef_Beam");

	// ##### CONTAINER - MODULE #####

	A->RegisterObjectType                                        ("RigDef_Module",               0, asOBJ_REF );
	A->RegisterInheritedSharedPtr<AS_RigDef_ModuleSharedPtr>     ("RigDef_ModuleSharedPtr",      "RigDef_Module");
	A->RegisterInheritedStdVector<AS_RigDef_ModuleSharedPtrArray>("RigDef_ModuleSharedPtrArray", "RigDef_ModuleSharedPtr");

	// Properties
	A->RegisterObjectProperty("RigDef_Module", "RigDef_NodeArray nodes", asOFFSET(RigDef::File::Module, nodes));
	A->RegisterObjectProperty("RigDef_Module", "RigDef_BeamArray beams", asOFFSET(RigDef::File::Module, beams));

	// ##### CONTAINER - FILE #####
	
	A->RegisterObjectType("RigDef_File",   0, asOBJ_REF );
	
	A->RegisterInheritedSharedPtr<AS_RigDef_FileSharedPtr>        ("RigDef_FileSharedPtr",   "RigDef_File");	

	// Properties
	A->RegisterObjectProperty("RigDef_File", "RigDef_ModuleSharedPtrArray modules",     asOFFSET(RigDef::File, modules));
	A->RegisterObjectProperty("RigDef_File", "RigDef_ModuleSharedPtr      root_module", asOFFSET(RigDef::File, root_module));

}

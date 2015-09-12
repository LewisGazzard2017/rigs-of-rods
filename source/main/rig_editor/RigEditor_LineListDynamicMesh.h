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
	@date   06/2015
*/

#pragma once

#include "RigEditor_DynamicMeshBase.h"
#include "RigEditor_ForwardDeclarations.h"

#include <OgreMaterialManager.h>

namespace RoR
{

namespace RigEditor
{

/// Simplistic wrapper of Ogre::ManualObject
class LineListDynamicMesh: public DynamicMeshBase
{
public:
    LineListDynamicMesh(
        RigEditor::Main* rig_editor,
        size_t estimate_line_count
    );
    virtual ~LineListDynamicMesh() {};

    void AddLine(Ogre::Vector3 const & start, Ogre::Vector3 const & end, Ogre::ColourValue const & color);
    void PY_AddLine(boost::python::object pos1, boost::python::object color1, boost::python::object pos2, boost::python::object color2);

    private:
    
    void CheckAndCreateMaterial(const char* mat_name);
};

} // namespace RigEditor

} // namespace RoR

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

#include "RigEditor_LineListDynamicMesh.h"

#include "PythonHelper.h"
#include "RigEditor_Main.h"

#include <OgreMaterialManager.h>
#include <OgreMaterial.h>
#include <OgreTechnique.h>
#include <OgrePass.h>
#include <OgreManualObject.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

#include <memory>

using namespace RoR;
using namespace RigEditor;

LineListDynamicMesh::LineListDynamicMesh(
        RigEditor::Main* rig_editor,
        size_t estimate_line_count
)
{
    static const char* material_name = "RigEditor_LineListDynamicMeshMaterial";
    this->CheckAndCreateMaterial(material_name);
    this->Initialize(rig_editor, estimate_line_count*2);

    // Initialize with dummy geometry
	m_dynamic_mesh->begin(material_name, Ogre::RenderOperation::OT_LINE_LIST);
	
		m_dynamic_mesh->position(Ogre::Vector3::UNIT_X);
		m_dynamic_mesh->colour(Ogre::ColourValue::Red);
		m_dynamic_mesh->position(Ogre::Vector3::UNIT_Y);
		m_dynamic_mesh->colour(Ogre::ColourValue::Green);
	
	m_dynamic_mesh->end();

    // Clear dummy geometry
    this->BeginUpdate();
    this->EndUpdate();
}

void LineListDynamicMesh::CheckAndCreateMaterial(const char* mat_name)
{
	if (Ogre::MaterialManager::getSingleton().resourceExists(mat_name))
	{
		return;
	}
	Ogre::MaterialPtr mat = static_cast<Ogre::MaterialPtr>(
		Ogre::MaterialManager::getSingleton().create(
			mat_name, 
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		)
	);

	mat->getTechnique(0)->getPass(0)->createTextureUnitState();
	mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureFiltering(Ogre::TFO_ANISOTROPIC);
	mat->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAnisotropy(3);
	mat->setLightingEnabled(false);
	mat->setReceiveShadows(false);
}

void LineListDynamicMesh::AddLine(Ogre::Vector3 const & start, Ogre::Vector3 const & end, Ogre::ColourValue const & color)
{
	m_dynamic_mesh->position(start);
	m_dynamic_mesh->colour(color);
	m_dynamic_mesh->position(end);
	m_dynamic_mesh->colour(color);
}

void LineListDynamicMesh::AddLine(Ogre::Vector3 const & start, Ogre::ColourValue const & color_start, Ogre::Vector3 const & end, Ogre::ColourValue const & color_end)
{
	m_dynamic_mesh->position(start);
	m_dynamic_mesh->colour(color_start);
	m_dynamic_mesh->position(end);
	m_dynamic_mesh->colour(color_end);
}

void LineListDynamicMesh::PY_AddLine(boost::python::object pos1, boost::python::object color1, boost::python::object pos2, boost::python::object color2)
{
	auto p1 = PythonHelper::Vector3_FromPython(pos1);
	auto p2 = PythonHelper::Vector3_FromPython(pos2);
	auto c1 = PythonHelper::Color_FromPython(color1);
	auto c2 = PythonHelper::Color_FromPython(color2);
	this->AddLine(p1, c1, p2, c2);
}


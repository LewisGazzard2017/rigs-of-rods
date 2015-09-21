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

#include "RigEditor_DynamicMeshBase.h"

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
#include <boost/python.hpp>

using namespace RoR;
using namespace RigEditor;

DynamicMeshBase::~DynamicMeshBase()
{
	if (m_dynamic_mesh)
	{
		delete m_dynamic_mesh;
		m_dynamic_mesh = nullptr;
	}
}

void DynamicMeshBase::Initialize(
	RigEditor::Main* rig_editor,
	size_t estimate_line_count
)
{
	assert(material_name != nullptr);
	assert(ogre_scene_manager != nullptr);

    auto* scene_manager = rig_editor->GetOgreSceneManager();

	m_dynamic_mesh = scene_manager->createManualObject();

	// Setup
	m_dynamic_mesh->estimateVertexCount(estimate_line_count * 2);
	m_dynamic_mesh->setCastShadows(false);
	m_dynamic_mesh->setDynamic(true);
	m_dynamic_mesh->setRenderingDistance(300);

    // Create scene node
    m_scene_node = scene_manager->createSceneNode();
    m_scene_node->attachObject(m_dynamic_mesh);

	m_root_scene_node = scene_manager->getRootSceneNode();
}

void DynamicMeshBase::BeginUpdate()
{
	m_dynamic_mesh->beginUpdate(0);
}

void DynamicMeshBase::EndUpdate()
{
	m_dynamic_mesh->end();
}

void DynamicMeshBase::DetachFromScene()
{
	if (m_scene_node->isInSceneGraph())
	{
		m_scene_node->getParent()->removeChild(m_scene_node);
	}
}

void DynamicMeshBase::PY_AttachToScene()
{
	assert(m_root_scene_node != nullptr);
	if (!m_scene_node->isInSceneGraph())
	{
		m_root_scene_node->addChild(m_scene_node);
	}
}

void DynamicMeshBase::SetPosition(Ogre::Vector3 pos)
{
    m_scene_node->setPosition(pos);
}

void DynamicMeshBase::PY_SetPosition(boost::python::object pos)
{
	this->SetPosition(PythonHelper::Vector3_FromPython(pos));
}

void DynamicMeshBase::SetOrientation(Ogre::Quaternion rot)
{
    m_scene_node->setOrientation(rot);
}

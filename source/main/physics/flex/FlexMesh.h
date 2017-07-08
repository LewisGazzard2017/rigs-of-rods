/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013-2017 Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

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

#pragma once

#include "RoRPrerequisites.h"

#include "GfxFlexingMeshes.h"

#include <OgreString.h>
#include <OgreEntity.h>
#include <OgreVector3.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreHardwareBuffer.h>

/// Generated mesh for wheels, see 'RigSpawner::CreateWheelVisuals()' - Used by truckfile sections "wheels", "wheels2", 
class FlexMesh: public RoR::FlexableMesh
{
public:

    FlexMesh(
        Ogre::String const& name,
        node_t* nds,
        int n1,
        int n2,
        int nstart,
        int nrays,
        Ogre::String const& face_material_name,
        Ogre::String const& band_material_name,
        bool rimmed = false,
        float rimratio = 1.f
    );

    ~FlexMesh();

    Ogre::Vector3 updateVertices();

    // FlexableMesh
    bool               FlexitPrepare() override { return true; }
    void               FlexitCompute() override;
    Ogre::Vector3      FlexitFinal() override;
    void               SetVisible(bool visible) {} // Nothing to do here

private:

    struct FlexMeshVertex
    {
        Ogre::Vector3 position;
        Ogre::Vector3 normal;
        Ogre::Vector2 texcoord;
    };

    // Wheel
    Ogre::Vector3     m_flexit_center;
    node_t*           m_all_nodes;
    int               m_num_rays;
    bool              m_is_rimmed;

    // Meshes
    Ogre::MeshPtr     m_mesh;
    Ogre::SubMesh*    m_submesh_wheelface;
    Ogre::SubMesh*    m_submesh_tiretread;
    Ogre::VertexDeclaration* m_vertex_format;
    Ogre::HardwareVertexBufferSharedPtr m_hw_vbuf;

    // Vertices
    FlexMeshVertex*   m_vertices;
    int*              m_vertex_nodes;

    // Indices
    unsigned short*   m_wheelface_indices;
    unsigned short*   m_tiretread_indices;
};

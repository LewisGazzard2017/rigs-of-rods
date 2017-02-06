/*
    This source file is part of Rigs of Rods
    Copyright 2005-2012 Pierre-Michel Ricordel
    Copyright 2007-2012 Thomas Fischer
    Copyright 2013+     Petr Ohlidal & contributors

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

#include "BeamData.h"

#include <OgreVector3.h>
#include <OgreMesh.h>
#include <OgreSubMesh.h>
#include <OgreHardwareBuffer.h>

/*
How traditional softbody visuals work
=====================================

1. User adds one or more "submesh" sections to a truckfile
   http://docs.rigsofrods.org/vehicle-creation/fileformat-truck/#submesh

2. RigDef::Parser reads truckfile and creates RigDef::Submesh instances

3. RigSpawner begins spawning the actor (class Beam: public rig_t).
   These fields come into play:
     int            rig_t::subtexcoords[MAX_SUBMESHES]; ---- Spawn-only:
     int            rig_t::subcabs[MAX_SUBMESHES];
     int            rig_t::free_sub;                 ---- Spawn-only: Submesh counter, used for indexing multiple other arrays.
	 int            rig_t::cabs[MAX_CABS*3];         ---- SIM: Used all over the place for collisions
 	 int            rig_t::free_cab;
     int            rig_t::subisback[MAX_SUBMESHES]; ---- Spawn-only: drives material assignment in FlexObj ctor.

	 Hierarchy:
	     free_sub ---- Master submesh index
		     subcabs[free_sub]      -> index to cabs ??
			 subtexcoords[free_sub] -> index to texcoords??
			 subisback[free_sub]    -> value: 0=no/1=yes/2=yes+trans

		 free_cab ---- cab node counter
		     cabs[free_cab] -> cab node index (node 0/3)

         free_collcab ---- collision triangle counter
		     collcabs[free_collcab] -> cab triangle (not node!) index
			 nodex[cabs[collcabs[i*3  ]]] -> cab triangle node 0
			 nodex[cabs[collcabs[i*3+1]]] -> cab triangle node 1
			 nodex[cabs[collcabs[i*3+2]]] -> cab triangle node 2
			 
   

*/

/// Texture coordinates for old-style actor body (the "cab")
struct CabTexcoord
{
	int    node_id;
	float  texcoord_u;
	float  texcoord_v;
};

/// Submesh for old-style actor body (the "cab")
struct CabSubmesh
{
	enum BackmeshType { BACKMESH_NONE, BACKMESH_OPAQUE, BACKMESH_TRANSPARENT };

	CabSubmesh(): backmesh_type(BACKMESH_NONE), texcoords_first(0), texcoords_last(0), cabs_first(0), cabs_last(0) {}
	size_t GetTriangleCount() { return (cabs_last - cabs_first) + 1; }

	BackmeshType    backmesh_type;
	size_t          texcoords_first, texcoords_last; // Indexes to shared texcoords array
	size_t          cabs_first, cabs_last; // Indexes to shared cab-triangles array
};

/// A visual mesh, forming a chassis for softbody actor
/// At most one instance is created per actor.
class FlexObj : public ZeroedMemoryAllocator
{
public:

    FlexObj(
        node_t* nds,
        std::vector<CabTexcoord>& texcoords,
        int numtriangles,
        int* triangles,
        std::vector<CabSubmesh>& submeshes,
        char* texname,
        char* name,
        char* backtexname,
        char* transtexname);

    ~FlexObj();

    //with normals
    Ogre::Vector3 updateVertices();
    //with normals
    Ogre::Vector3 updateShadowVertices();
    Ogre::Vector3 flexit();
    void scale(float factor);

private:

    struct CabCoVertice_t
    {
        Ogre::Vector3 vertex;
        Ogre::Vector3 normal;
        Ogre::Vector2 texcoord;
    };

    struct CabPosVertice_t
    {
        Ogre::Vector3 vertex;
    };

    struct CabNorVertice_t
    {
        Ogre::Vector3 normal;
        Ogre::Vector2 texcoord;
    };

	///find the zeroed id of the node v in the context of the tidx triangle
	int findID(int tidx, int v, std::vector<CabSubmesh>& submeshes);

    Ogre::MeshPtr msh;
    std::vector<Ogre::SubMesh*> m_submeshes;
    Ogre::VertexDeclaration* decl;
    Ogre::HardwareVertexBufferSharedPtr vbuf;

    size_t nVertices;
    size_t vbufCount;

    //shadow
    union
    {
        float* shadowposvertices;
        CabPosVertice_t* coshadowposvertices;
    };

    union
    {
        float* shadownorvertices;
        CabNorVertice_t* coshadownorvertices;
    };

    union
    {
        float* vertices;
        CabCoVertice_t* covertices;
    };

    //nodes
    int* nodeIDs;

    size_t ibufCount;
    unsigned short* faces;
    node_t* nodes;

    float* sref;
};

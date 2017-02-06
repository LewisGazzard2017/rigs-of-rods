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

#include "FlexObj.h"

#include <Ogre.h>

using namespace Ogre;

FlexObj::FlexObj(
    node_t*                   all_nodes,
    std::vector<CabTexcoord>& texcoords,
    int                       cabtriangle_count,
    int*                      cabtriangle_nodes,
    std::vector<CabSubmesh>&  submesh_defs,
    char*                     texture_name,
    char*                     mesh_name,
    char*                     backmesh_tex_name,
    char*                     transmesh_tex_name
    )
{
    // ## 3-phase process:
    // ##    1. Create local vertex/index buffers + update structures
    // ##    2. Call update routine to initialize the buffers
    // ##    3. Create actual OGRE 3D resources

    // Populate vertex buffer + helper node array
    const size_t vertex_count = texcoords.size();
    m_vertex_buffer.resize(vertex_count);
    m_vertex_nodes.resize(vertex_count);

    for (size_t i = 0; i < vertex_count; ++i)
    {
        m_vertex_nodes[i] = &all_nodes[texcoords[i].node_id];
        m_vertex_buffer[i].texcoord = Ogre::Vector2(texcoords[i].texcoord_u, texcoords[i].texcoord_v);
    }

    // Populate index buffer
    //   Input: Array of {node ID + texcoord} entries. 
    //   Task: We need to look up vertex positions for the referenced nodes.
    for (CabSubmesh def: submesh_defs)
    {
        for (size_t i = def.texcoords_first; i <= def.texcoords_last; ++i)
        {
            Ogre::uint16 value = 0; // Initial value. TODO: Log a warning if node isn't found
            for (size_t j = def.cabs_first * 3; j <= def.cabs_last * 3; ++j) // Loop submesh's cab-triangle nodes
            {
                if (texcoords[i].node_id == cabtriangle_nodes[j]) // We found the node, use it's position.
                    value = j;
            }
            m_index_buffer.push_back(value);
        }
    }

    // Old logic, not sure what it is ~only_a_ptr, 02/2017
    for (int i = 0; i < cabtriangle_count; ++i)
    {
        Ogre::Vector3 base_pos  = m_vertex_nodes[m_index_buffer[i*3]]->RelPosition;
        Ogre::Vector3 tri_side1 = m_vertex_nodes[m_index_buffer[(i*3)+1]]->RelPosition - base_pos;
        Ogre::Vector3 tri_side2 = m_vertex_nodes[m_index_buffer[(i*3)+2]]->RelPosition - base_pos;
        m_sref.push_back(tri_side1.crossProduct(tri_side2).length() * 2.f);
    }

    this->UpdateVertices();

    // Create mesh
    msh = Ogre::MeshManager::getSingleton().createManual(mesh_name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); // TODO: use custom resourcegroup!
    msh->_setBounds(AxisAlignedBox(-100,-100,-100,100,100,100), true);
    
    // Set mesh's vertex format
    decl = msh->sharedVertexData->vertexDeclaration;
    size_t offset = 0;
    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
    offset += VertexElement::getTypeSize(VET_FLOAT3);
    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
    offset += VertexElement::getTypeSize(VET_FLOAT2);

    // Create shared vertex buffer
    msh->sharedVertexData = new VertexData();
    msh->sharedVertexData->vertexCount = vertex_count;
    vbuf = HardwareBufferManager::getSingleton().createVertexBuffer(
        offset, vertex_count, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);

    // Upload the vertex data to the card
    vbuf->writeData(0, vbuf->getSizeInBytes(), m_vertex_buffer.data(), true);

    // Set vertex buffer binding so buffer 0 is bound to our vertex buffer
    VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding;
    bind->setBinding(0, vbuf);

    // Create submeshes
    m_submeshes.reserve(submesh_defs.size());
    for (CabSubmesh& submesh_def : submesh_defs)
    {
        // Create index buffer
        size_t index_count = submesh_def.GetTriangleCount() * 3;
        HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
             HardwareIndexBuffer::IT_16BIT,
             index_count,
             HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        // Populate index buffer
        void* index_data = m_index_buffer.data() + (submesh_def.cabs_first * 3);
        ibuf->writeData(0, ibuf->getSizeInBytes(), index_data, true);

        Ogre::SubMesh* submesh = msh->createSubMesh();
        submesh->useSharedVertices = true;
        submesh->indexData->indexBuffer = ibuf;
        submesh->indexData->indexCount = index_count;
        submesh->indexData->indexStart = 0;

        switch (submesh_def.backmesh_type)
		{
		case CabSubmesh::BACKMESH_OPAQUE:       submesh->setMaterialName(backmesh_tex_name);  break;
		case CabSubmesh::BACKMESH_TRANSPARENT:  submesh->setMaterialName(transmesh_tex_name); break;
		default:                                submesh->setMaterialName(texture_name);
		}

		m_submeshes.push_back(submesh);
    }

    // Notify Mesh object that it has been loaded
    msh->load();

    // Assign fields
    triangleCount = cabtriangle_count;
    vbufCount = vertex_count;
    ibufCount = 3* cabtriangle_count;

    }

#if 0


    ////// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


//FlexObj::FlexObj(node_t *nds, std::vector<CabTexcoord>& texcoords, int numtriangles, 
//	             int* triangles, std::vector<CabSubmesh>& submesh_defs, 
//	             char* texname, char* name, char* backtexname, char* transtexname)

    unsigned int i;
    int triangleCount = numtriangles;

    //gEnv->sceneManager=gEnv->sceneManager;
    //nodes=nds;
    /// Create the mesh via the MeshManager
    //msh = MeshManager::getSingleton().createManual(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    /// Create submeshes
///	m_submeshes.reserve(submesh_defs.size());
///    for (size_t j=0; j<submesh_defs.size(); j++)
///    {
///		Ogre::SubMesh* submesh = msh->createSubMesh();
///        switch (submesh_defs[j].backmesh_type)
///		{
///		case CabSubmesh::BACKMESH_OPAQUE:       submesh->setMaterialName(backtexname);  break;
///		case CabSubmesh::BACKMESH_TRANSPARENT:  submesh->setMaterialName(transtexname); break;
///		default:                                submesh->setMaterialName(texname);
///		}
///		m_submeshes.push_back(submesh);
///    };

////    /// Define the vertices (8 vertices, each consisting of 3 groups of 3 floats
////    nVertices = texcoords.size();
////    vbufCount = (2*3+2)*nVertices;
////    vertices=(float*)malloc(vbufCount*sizeof(float));
////    //shadow
////    shadownorvertices=(float*)malloc(nVertices*(3+2)*sizeof(float));
////    shadowposvertices=(float*)malloc(nVertices*3*2*sizeof(float));
////    nodeIDs=(int*)malloc(nVertices*sizeof(int));
////
    
///    for (i=0; i<nVertices; i++)
///    {
///        nodeIDs[i] = texcoords[i].node_id; //define node ids
///		covertices[i].texcoord=Vector2(texcoords[i].texcoord_u, texcoords[i].texcoord_v); //textures coordinates
///    }

 //-//   /// Define triangles
 //-//   /// The values in this table refer to vertices in the above table
 //-//   ibufCount = 3*numtriangles;
 //-//   faces=(unsigned short*)malloc(ibufCount*sizeof(unsigned short));
 //-//   for (i=0; i<ibufCount; i++)
 //-//   {
 //-//       faces[i]=findID(i/3, triangles[i], submesh_defs);
 //-//   }

//-//    sref=(float*)malloc(numtriangles*sizeof(float));
//-//
//-//    for (i=0; i<(unsigned int)numtriangles;i++)
//-//    {
//-//        Vector3 v1, v2;
//-//        v1=nodes[nodeIDs[faces[i*3+1]]].RelPosition-nodes[nodeIDs[faces[i*3]]].RelPosition;
//-//        v2=nodes[nodeIDs[faces[i*3+2]]].RelPosition-nodes[nodeIDs[faces[i*3]]].RelPosition;
//-//        v1=v1.crossProduct(v2);
//-//        sref[i]=v1.length()*2.0;
//-//    }


    //update coords
    updateVertices();



//    /// Create vertex data structure for vertices shared between submeshes
//    msh->sharedVertexData = new VertexData();
//    msh->sharedVertexData->vertexCount = nVertices;


/////    /// Create declaration (memory format) of vertex data
/////    decl = msh->sharedVertexData->vertexDeclaration;
/////    size_t offset = 0;
/////    decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
/////    offset += VertexElement::getTypeSize(VET_FLOAT3);
/////    decl->addElement(0, offset, VET_FLOAT3, VES_NORMAL);
/////    offset += VertexElement::getTypeSize(VET_FLOAT3);
///////        decl->addElement(0, offset, VET_FLOAT3, VES_DIFFUSE);
///////        offset += VertexElement::getTypeSize(VET_FLOAT3);
/////    decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);
/////    offset += VertexElement::getTypeSize(VET_FLOAT2);


 //-//   /// Allocate vertex buffer of the requested number of vertices (vertexCount)
 //-//   /// and bytes per vertex (offset)
 //-//   vbuf =
 //-//     HardwareBufferManager::getSingleton().createVertexBuffer(
 //-//         offset, msh->sharedVertexData->vertexCount, HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
 //-//
 //-//   /// Upload the vertex data to the card
 //-//   vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
 //-//
 //-//
 //-//   /// Set vertex buffer binding so buffer 0 is bound to our vertex buffer
 //-//   VertexBufferBinding* bind = msh->sharedVertexData->vertexBufferBinding;
 //-//   bind->setBinding(0, vbuf);

    /// Set parameters of the submeshes
    for (size_t j=0; j<m_submeshes.size(); j++)
    {
        size_t index_count = submesh_defs[j].GetTriangleCount() * 3;
        m_submeshes[j]->useSharedVertices = true;
        /// Allocate index buffer of the requested number of vertices (ibufCount)
        HardwareIndexBufferSharedPtr ibuf = HardwareBufferManager::getSingleton().createIndexBuffer(
             HardwareIndexBuffer::IT_16BIT,
             index_count,
             HardwareBuffer::HBU_STATIC_WRITE_ONLY);

        /// Upload the index data to the card
		unsigned short* faces_ptr = &faces[submesh_defs[j].cabs_first * 3];
        ibuf->writeData(0, ibuf->getSizeInBytes(), faces_ptr, true);
        m_submeshes[j]->indexData->indexBuffer = ibuf;
        m_submeshes[j]->indexData->indexCount = index_count;
        m_submeshes[j]->indexData->indexStart = 0;
    }


    /// Set bounding information (for culling)
    
    //msh->_setBoundingSphereRadius(100);


    /// Notify Mesh object that it has been loaded
    msh->load();
}

#endif

void FlexObj::scale(float factor)
{
    for (int i=0; i<triangleCount;i++)
    {
        sref[i] *= factor;
    }
}

//find the zeroed id of the node v in the context of the tidx triangle // ### TODO: Eliminate this weirdness!
int FlexObj::findID(int tidx, int v, std::vector<CabSubmesh>& submeshes)
{
    //first: find the context from the triangle index tidx
    size_t context;
    for (context=0; context<submeshes.size()+1; context++) 
	{
		if (static_cast<unsigned int>(tidx) < (submeshes[context].cabs_last + 1))
		{
			context--;
			break;
		}
	}
    //okay, now search in the vertice block corresponding to the context
	for (size_t j=(submeshes[context].texcoords_last+1); j<(submeshes[context+1].texcoords_last+1); j++)
	{
		if (nodeIDs[j]==v)
			return j;
	}
    return 0;
}

/*Vector3 FlexObj::updateVertices()
{
    unsigned int i;
    Vector3 center;
    center=(nodes[nodeIDs[0]].Position+nodes[nodeIDs[1]].Position)/2.0;
    for (i=0; i<nVertices; i++)
    {
        covertices[i].vertex=nodes[nodeIDs[i]].Position-center;
        //normals
        covertices[i].normal=covertices[i].vertex;
        covertices[i].normal.normalise();
    }

    return center;
}
*/

void FlexObj::UpdateVertices()
{
    const size_t vertex_count = m_vertex_buffer.size();
    const size_t index_count = m_index_buffer.size();

    Ogre::Vector3 center = (m_vertex_nodes[0]->AbsPosition + m_vertex_nodes[1]->AbsPosition) / 2.f;
    for (size_t i = 0; i < vertex_count; ++i)                                     // For each vertex...
    {
        m_vertex_buffer[i].position = m_vertex_nodes[i]->AbsPosition - center;    // Set position
        m_vertex_buffer[i].normal = Ogre::Vector3::ZERO;                          // Reset normal
    }

    // Accumulate triangle normals
    for (size_t i = 0; i < index_count; ++i)
    {
        const size_t node1_idx = m_index_buffer[i*3];
        const size_t node2_idx = m_index_buffer[(i*3)+1];
        const size_t node3_idx = m_index_buffer[(i*3)+2];

        Ogre::Vector3 base_pos  = m_vertex_nodes[node1_idx]->RelPosition;
        Ogre::Vector3 tri_side1 = m_vertex_nodes[node2_idx]->RelPosition - base_pos;
        Ogre::Vector3 tri_side2 = m_vertex_nodes[node3_idx]->RelPosition - base_pos;
        Ogre::Vector3 cross = tri_side1.crossProduct(tri_side2);
        float s = cross.length();

        if (s > m_sref[i]) // Avoid large tris
        {
            Ogre::Vector3 vert_base_pos = m_vertex_buffer[node1_idx].position;
            m_vertex_buffer[node2_idx].position = vert_base_pos + Ogre::Vector3(0.1,0,0);
            m_vertex_buffer[node3_idx].position = vert_base_pos + Ogre::Vector3(0,0,0.1);
        }

        if (s == 0) // Don't divide by zero!
            continue;

        Ogre::Vector3 contrib = cross/s;
        m_vertex_buffer[node1_idx].normal += contrib;
        m_vertex_buffer[node2_idx].normal += contrib;
        m_vertex_buffer[node3_idx].normal += contrib;
    }

    // Normalise
    for (size_t i = 0; i < vertex_count; ++i)
    {
        m_vertex_buffer[i].normal = approx_normalise(m_vertex_buffer[i].normal);
    }
}

//with normals
#if 0
Vector3 FlexObj::updateVertices()
{
    unsigned int i;
    Vector3 center;
    center=(nodes[nodeIDs[0]].AbsPosition+nodes[nodeIDs[1]].AbsPosition)/2.0;
    for (i=0; i<nVertices; i++)
    {
        //set position
        covertices[i].vertex=nodes[nodeIDs[i]].AbsPosition-center;
        //reset normals
        covertices[i].normal=Vector3::ZERO;
    }
    //accumulate normals per triangle
    for (i=0; i<ibufCount/3; i++)
    {
        Vector3 v1, v2;
        v1=nodes[nodeIDs[faces[i*3+1]]].AbsPosition-nodes[nodeIDs[faces[i*3]]].AbsPosition;
        v2=nodes[nodeIDs[faces[i*3+2]]].AbsPosition-nodes[nodeIDs[faces[i*3]]].AbsPosition;
        v1=v1.crossProduct(v2);
        float s=v1.length();
        //avoid large tris
        if (s>sref[i])
        {
            covertices[faces[i*3+1]].vertex=covertices[faces[i*3]].vertex+Vector3(0.1,0,0);
            covertices[faces[i*3+2]].vertex=covertices[faces[i*3]].vertex+Vector3(0,0,0.1);
        }
        //v1.normalise();
        if (s == 0)
            continue;
        v1=v1/s;
        covertices[faces[i*3]].normal+=v1;
        covertices[faces[i*3+1]].normal+=v1;
        covertices[faces[i*3+2]].normal+=v1;
    }
    //normalize
    for (i=0; i<nVertices; i++)
    {
        covertices[i].normal = approx_normalise(covertices[i].normal);
    }

    return center;
}

#endif

//with normals
Vector3 FlexObj::updateShadowVertices()
{
    Vector3 center = (nodes[nodeIDs[0]].AbsPosition + nodes[nodeIDs[1]].AbsPosition) / 2.0;

    for (unsigned int i=0; i<nVertices; i++)
    {
        //set position
        coshadowposvertices[i].vertex=nodes[nodeIDs[i]].AbsPosition-center;
        coshadowposvertices[i+nVertices].vertex=nodes[nodeIDs[i]].AbsPosition-center;
        //reset normals
        coshadownorvertices[i].normal=Vector3::ZERO;
    }
    //accumulate normals per triangle
    for (unsigned int i=0; i<ibufCount/3; i++)
    {
        Vector3 v1, v2;
        v1=nodes[nodeIDs[faces[i*3+1]]].AbsPosition-nodes[nodeIDs[faces[i*3]]].AbsPosition;
        v2=nodes[nodeIDs[faces[i*3+2]]].AbsPosition-nodes[nodeIDs[faces[i*3]]].AbsPosition;
        v1=v1.crossProduct(v2);
        float s=v1.length();
        //avoid large tris
        if (s>sref[i])
        {
            coshadowposvertices[faces[i*3+1]].vertex=coshadowposvertices[faces[i*3]].vertex+Vector3(0.1,0,0);
            coshadowposvertices[faces[i*3+2]].vertex=coshadowposvertices[faces[i*3]].vertex+Vector3(0,0,0.1);
            coshadowposvertices[faces[i*3+1]+nVertices].vertex=coshadowposvertices[faces[i*3+1]].vertex;
            coshadowposvertices[faces[i*3+2]+nVertices].vertex=coshadowposvertices[faces[i*3+2]].vertex;
        }
        //v1.normalise();
        if (s == 0)
            continue;
        v1=v1/s;

        coshadownorvertices[faces[i*3]].normal+=v1;
        coshadownorvertices[faces[i*3+1]].normal+=v1;
        coshadownorvertices[faces[i*3+2]].normal+=v1;
    }
    //normalize
    for (unsigned int i=0; i<nVertices; i++)
    {
        coshadownorvertices[i].normal = approx_normalise(coshadownorvertices[i].normal);
        //texcoords
        coshadownorvertices[i].texcoord=covertices[i].texcoord;
    }

    return center;
}

Vector3 FlexObj::flexit()
{
    Vector3 center(Vector3::ZERO);

    center=updateVertices();
    //vbuf->lock(HardwareBuffer::HBL_NORMAL);
    vbuf->writeData(0, vbuf->getSizeInBytes(), vertices, true);
    //vbuf->unlock();
    //msh->sharedVertexData->vertexBufferBinding->getBuffer(0)->writeData(0, vbuf->getSizeInBytes(), vertices, true);

    return center;
}

FlexObj::~FlexObj()
{
    if (!msh.isNull()) msh->unload();

    if (vertices          != nullptr) { free (vertices); }
    if (shadownorvertices != nullptr) { free (shadownorvertices); }
    if (shadowposvertices != nullptr) { free (shadowposvertices); }
    if (nodeIDs           != nullptr) { free (nodeIDs); }
    if (faces             != nullptr) { free (faces); }
    if (sref              != nullptr) { free (sref); }
}

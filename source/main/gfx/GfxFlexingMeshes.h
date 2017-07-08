/*
    This source file is part of Rigs of Rods

    Copyright 2015-2017 Petr Ohlidal & contributors

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

/// @file
/// @author Petr Ohlidal, 07/2017
/// @brief  CPU-computed flexing meshes which deform along with attached N/B

/// ------------------------ Refactor of flexbodies ----- 07/2017 -----------------------------
/// RoR has many types of flexing mesh, all of which are mostly copy-pasted (ugly!) code.
/// These fall to 2 categories: DIRECT - vertex pos matches node pos; FLEX - vertex position is derived from 4 nodes: origin, X, Y, Z
///  * FlexAirfoil    - DIRECT
///  * FlexBody       - FLEX   - Used by truckfile sections "flexbodies" and "flexbodywheels". Built using `FlexFactory`
///  * FlexMesh       - DIRECT - Generated mesh for wheels, see `RigSpawner::CreateWheelVisuals()`; Used by truckfile sections "wheels", "wheels2",
///  * FlexMeshWheel  - DIRECT
///  * FlexObj        - DIRECT
///
/// Recently, flexbodies began to cause issues (the famous "node XYZ/XYZ not found" message)
/// for no apparent reason. To ease maintenance, this refactor was started.
///
/// Plan: Introduce one compilation unit "GfxFlexingMeshes" ("Gfx*" because it's mostly OGRE-manipulating logic)
/// which defines the interface `FlexableMesh` (previously `Flexable`) and hosts the implementations (unified)
/// Note: Class `FlexFactory` will be extended in-place merged here at the end of the refactor.
///
/// This header will remain here until the refactor is done ~ only_a_ptr, 07/2017
/// -------------------------------------------------------------------------------------------

#pragma once

#include <OgreVector3.h>

namespace RoR {

/// Encapsulates an `Ogre::Mesh` instance and updates it's deformation
/// NOTE: OGRE engine uses `Ogre::Mesh/SubMesh` as actual data and `Ogre::Entity/SubEntity` as linked in-scene instances. This class manges the `Ogre::Mesh`.
class FlexableMesh
{
public:
    virtual ~FlexableMesh() {}

    virtual bool              FlexitPrepare() = 0;
    virtual void              FlexitCompute() = 0;
    virtual Ogre::Vector3     FlexitFinal() = 0;
    virtual void              SetVisible(bool visible) = 0; // TODO: Wrong design! Instead of carrying an 'isVisible' flag, manage arrays of visible and not-visible objects! ~ only_a_ptr
};

} // namespace RoR

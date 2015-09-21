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
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Rigs of Rods. If not, see <http://www.gnu.org/licenses/>.
*/

/** 
	@file   
	@date   07/2014
*/

#pragma once

#include <boost/python/object.hpp>
	// Must be included before system headers (Python restriction)

#include "RigDef_Prerequisites.h"
#include "RigEditor_Types.h"
#include "RoRPrerequisites.h"

#include <OgreMatrix4.h>

namespace RoR
{

namespace RigEditor
{

/**
* Python wrapper for camera + viewport
*/
class CameraWrapper
{
public:

	CameraWrapper():
		m_camera(nullptr)
	{}

	void    SetCamera(Ogre::Camera* cam) { m_camera = cam; }
	
	bool    PY_IsModeOrtho();
	void    PY_ToggleModeOrtho();
	void    PY_YawDegrees(float deg);
	void    PY_RollDegrees(float deg);
	void    PY_PitchDegrees(float deg);
	void    PY_SetAutoAspectRatio(bool v);
	void    PY_SetModeOrtho(bool do_ortho);
	void    PY_SetFarClipDistance(float v);
	void    PY_SetNearClipDistance(float v);
	void    PY_SetOrthoWindowWidth(float width);
	void    PY_SetFOVyDegrees(float fov_y_degrees);
	void    PY_LookAt(boost::python::object world_pos);
	void    PY_MoveRelative(boost::python::object offset);
	void    PY_SetPosition(boost::python::object world_pos);
	float   PY_PointZDistance(boost::python::object world_pos);

	// Returns a dictionary: { resolved=Bool, x=int, y=int } XY = pixels from top left corner of viewport
	boost::python::object PY_ConvertWorldToScreenPosition(boost::python::object world_pos);

	// Returns a dictionary: {x,y,z}
	// screen_pos: Vector2: Pixels from top left corner of viewport.
	// pivot: Vector3: A point in 3d space specifying a 'plane' (geometry) to place the point onto. 
	//     Think of it as a mounting-point for a glass pane which you'll paint your position onto.
	boost::python::object PY_ConvertScreenToWorldPosition(boost::python::object screen_pos, boost::python::object pivot);

protected:

	Ogre::Camera* m_camera;
};

} // namespace RigEditor

} // namespace RoR

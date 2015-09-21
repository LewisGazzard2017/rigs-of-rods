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

#include "RigEditor_CameraWrapper.h"

#include "PythonHelper.h"

#include <OgreCamera.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>

using namespace RoR;
using namespace RoR::RigEditor;

void CameraWrapper::PY_SetModeOrtho(bool do_ortho)
{
    if (do_ortho != this->PY_IsModeOrtho())
    {
        if (do_ortho)
        {
            m_camera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
        }
        else
        {
            m_camera->setProjectionType(Ogre::PT_PERSPECTIVE);
        }
    }
}

bool CameraWrapper::PY_IsModeOrtho()
{
    return m_camera->getProjectionType() == Ogre::PT_ORTHOGRAPHIC;
}

void CameraWrapper::PY_ToggleModeOrtho()
{
    this->PY_SetModeOrtho(!this->PY_IsModeOrtho());
}

void CameraWrapper::PY_SetOrthoWindowWidth(float width)
{
	m_camera->setOrthoWindowWidth(width);
}

void CameraWrapper::PY_LookAt(boost::python::object world_pos)
{
    m_camera->lookAt(PythonHelper::Vector3_FromPython(world_pos));
}

void CameraWrapper::PY_SetPosition(boost::python::object world_pos)
{
    m_camera->setPosition(PythonHelper::Vector3_FromPython(world_pos));
}

float CameraWrapper::PY_PointZDistance(boost::python::object py_world_pos)
{
	Ogre::Vector3 world_pos = PythonHelper::Vector3_FromPython(py_world_pos);
	return (m_camera->getViewMatrix() * world_pos).z * -1;
}

void CameraWrapper::PY_YawDegrees(float deg)
{
    m_camera->yaw(Ogre::Degree(deg));
}

void CameraWrapper::PY_RollDegrees(float deg)
{
    m_camera->roll(Ogre::Degree(deg));
}

void CameraWrapper::PY_PitchDegrees(float deg)
{
    m_camera->pitch(Ogre::Degree(deg));
}

void CameraWrapper::PY_MoveRelative(boost::python::object offset)
{
    m_camera->moveRelative(PythonHelper::Vector3_FromPython(offset));
}

void CameraWrapper::PY_SetFarClipDistance(float v)
{
	m_camera->setFarClipDistance(v);
}

void CameraWrapper::PY_SetNearClipDistance(float v)
{
	m_camera->setNearClipDistance(v);
}

void CameraWrapper::PY_SetFOVyDegrees(float fov_y_degrees)
{
	m_camera->setFOVy(Ogre::Degree(fov_y_degrees));
}

/**
* @author http://www.ogre3d.org/forums/viewtopic.php?p=463232#p463232
* @author http://www.ogre3d.org/tikiwiki/tiki-index.php?page=GetScreenspaceCoords&structure=Cookbook
*/
boost::python::object CameraWrapper::PY_ConvertWorldToScreenPosition(boost::python::object world_pos)
{
    Ogre::Vector3 _world_position = PythonHelper::Vector3_FromPython(world_pos);
    
    boost::python::object ret_val;

	// Transform position: world space -> view space
	Ogre::Vector3 view_space_pos = m_camera->getViewMatrix(true) * _world_position;

	// Check if the position is in front of the camera
	if (view_space_pos.z < 0.f)
	{
		// Transform: view space -> clip space [-1, 1]
		Ogre::Vector3 clip_space_pos = m_camera->getProjectionMatrix() * view_space_pos;

		// Transform: clip space [-1, 1] -> to [0, 1]
		float screen_space_pos_x = (clip_space_pos.x / 2.f) + 0.5f;
		float screen_space_pos_y = 1 - ((clip_space_pos.y / 2.f) + 0.5f);

		// Transform: clip space -> absolute pixel coordinates
		Ogre::Viewport* viewport = m_camera->getViewport();
		int out_x = static_cast<int>(screen_space_pos_x * viewport->getActualWidth());
		int out_y = static_cast<int>(screen_space_pos_y * viewport->getActualHeight());

        ret_val['x'] = out_x;
        ret_val['y'] = out_y;
        ret_val["resolved"] = true;
	}
	else
	{
		ret_val['x'] = 0;
        ret_val['y'] = 0;
        ret_val["resolved"] = false;
	}
    return ret_val;
}

boost::python::object CameraWrapper::PY_ConvertScreenToWorldPosition(boost::python::object _screen_pos, boost::python::object _pivot)
{
    Ogre::Vector3 pivot   = PythonHelper::Vector3_FromPython(_pivot);
    Vector2int screen_pos = PythonHelper::Vector2int_FromPython(_screen_pos);

	// NOTE:
	// Screen pixel coordinates depend on your resolution:
	//     * start at top left corner of the screen (X:0, Y:0)
	//     * end in bottom right (for example X:1024, Y:768)
	// Clip space coordinates are always in range [-1, 1]:
	//     * start in the middle of the screen (X:0, Y:0, Z:0)
	//     * Bottom left corner of screen is (X:-1, Y:-1, Z:0)
	//     * Top right corner of screen is (X:1, Y:1, Z:0)

	// Transform: pixel coordinates -> clip space
	Ogre::Viewport* viewport = m_camera->getViewport();
	Ogre::Vector3 output(
			(((static_cast<float>(screen_pos.x) / static_cast<float>(viewport->getActualWidth()) ) *  2.f) - 1.f),
			(((static_cast<float>(screen_pos.y) / static_cast<float>(viewport->getActualHeight())) * -2.f) + 1.f),
			0.f
		);
        
    Ogre::Matrix4 projection_matrix = m_camera->getProjectionMatrix();
    Ogre::Matrix4 view_matrix       = m_camera->getViewMatrix();

	// Set point's depth, defined by pivot
	Ogre::Vector3 clip_space_pivot = projection_matrix * view_matrix * pivot; // Transform pivot: world space -> clip space
	output.z = clip_space_pivot.z;

	// Transform clip space -> view space
	output = projection_matrix.inverse() * output;

	// Transform view space -> world space
	Ogre::Vector3 result = view_matrix.inverse() * output;

	// Return python object
	boost::python::object py_result;
	py_result['x'] = result.x;
	py_result['y'] = result.y;
	py_result['z'] = result.z;
	return py_result;
}

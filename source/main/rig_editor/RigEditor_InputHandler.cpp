/*
	This source file is part of Rigs of Rods
	Copyright 2005-2012 Pierre-Michel Ricordel
	Copyright 2007-2012 Thomas Fischer
	Copyright 2013-2014 Petr Ohlidal

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
	@file   RigEditor_InputHandler.cpp
	@date   06/2014
	@author Petr Ohlidal
*/

#include "RigEditor_InputHandler.h"

#include "Application.h"
#include "GUIManager.h"
#include "InputEngine.h"

using namespace RoR;
using namespace RoR::RigEditor;

// ================================================================================
// OIS Keyboard listener
// ================================================================================

bool InputHandler::keyPressed( const OIS::KeyEvent &arg )
{
	// Inform GUI first
	if (RoR::Application::GetGuiManager()->keyPressed(arg))
	{
		return true;
	}

	// Invoke python listener
	m_listener.attr("key_pressed_callback")(arg.key, arg.text);

	return true;
}

bool InputHandler::keyReleased( const OIS::KeyEvent &arg )
{
	// Inform GUI first
	if (RoR::Application::GetGuiManager()->keyReleased(arg))
	{
		return true;
	}

	// Invoke python listener
	m_listener.attr("key_released_callback")(arg.key, arg.text);

	return true;
}

// ================================================================================
// OIS Mouse listener
// ================================================================================
/* // TODO: Pythonize
bool InputHandler::mouseMoved( const OIS::MouseEvent &mouse_event )
{
	MyGUI::InputManager::getInstance().injectMouseMove(
		mouse_event.state.X.abs, 
		mouse_event.state.Y.abs,
		mouse_event.state.Z.abs
		);

	m_mouse_motion_event.AddRelativeMove(mouse_event.state.X.rel, mouse_event.state.Y.rel, mouse_event.state.Z.rel);
	m_mouse_motion_event.abs_x = mouse_event.state.X.abs;
	m_mouse_motion_event.abs_y = mouse_event.state.Y.abs;
	m_mouse_motion_event.abs_wheel = mouse_event.state.Z.abs;

	return true;
}

bool InputHandler::mousePressed( const OIS::MouseEvent &mouse_event, OIS::MouseButtonID button_id )
{
	bool handled_by_gui = MyGUI::InputManager::getInstance().injectMousePress(
		mouse_event.state.X.abs, 
		mouse_event.state.Y.abs,
		MyGUI::MouseButton::Enum(button_id)
		);

	m_mouse_button_event.ButtonPressInputReceived(handled_by_gui);
	if (handled_by_gui)
	{
		return true;
	}

	switch (button_id)
	{
	case OIS::MB_Right:
		m_mouse_button_event.RightButtonDown();
		break;
	case OIS::MB_Left:
		m_mouse_button_event.LeftButtonDown();
		break;
	case OIS::MB_Middle:
		m_mouse_button_event.MiddleButtonDown();
		break;
	default:
		assert(false && "Invalid OIS::MouseButtonID");
	}

	return true;
}

bool InputHandler::mouseReleased( const OIS::MouseEvent &mouse_event, OIS::MouseButtonID button_id )
{
	bool handled_by_gui = MyGUI::InputManager::getInstance().injectMouseRelease(
		mouse_event.state.X.abs, 
		mouse_event.state.Y.abs,
		MyGUI::MouseButton::Enum(button_id)
		);

	m_mouse_button_event.ButtonReleaseInputReceived(handled_by_gui);
	if (handled_by_gui)
	{
		return true;
	}

	switch (button_id)
	{
	case OIS::MB_Right:
		m_mouse_button_event.RightButtonUp();
		break;
	case OIS::MB_Left:
		m_mouse_button_event.LeftButtonUp();
		break;
	case OIS::MB_Middle:
		m_mouse_button_event.MiddleButtonUp();
		break;
	default:
		assert(false && "Invalid OIS::MouseButtonID");
	}

	return true;
}*/

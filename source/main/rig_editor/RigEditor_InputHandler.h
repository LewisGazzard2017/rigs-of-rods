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
	@file   RigEditor_InputHandler.h
	@date   06/2014
	@author Petr Ohlidal
*/

#pragma once

#include "BitFlags.h"
#include "RigEditor_Types.h"

#include <OISKeyboard.h>
#include <OISMouse.h>
#include <bitset>
#include <boost/python/object.hpp>

namespace RoR
{

namespace RigEditor
{

/** Editor-specific input handler
* NOTE: class InputEngine is not useful because it's too tightly-coupled with simulation logic.
*/
class InputHandler:
	public OIS::MouseListener,
	public OIS::KeyListener
{
public:
	InputHandler() {};

	void SetPythonInputListener(boost::python::object listener) { m_listener = listener; }

	// OIS::KeyListener 
	bool keyPressed( const OIS::KeyEvent &arg );
	bool keyReleased( const OIS::KeyEvent &arg );

	// OIS MouseListener
	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

private:

	boost::python::object m_listener;
};

} // namespace RigEditor

} // namespace RoR

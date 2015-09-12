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
	@date   08/2015
	@author Petr Ohlidal
*/

#pragma once

#include "RoRPrerequisites.h"

namespace RoR
{

namespace RigEditor
{

// Forward decl
class  Main;
struct ScriptEngineImpl; 
	// Pimpl, because <boost/python/object.hpp> included here won't compile for some::reason<T>.

class ScriptEngine
{
public:
	ScriptEngine();
	~ScriptEngine();

	void Bootstrap();

	/// @return True on successful execution
	bool EnterRigEditor();

	void ShutDown();

protected:

	std::string GetConfigPath(const char* config_key);

	Ogre::Log*                       m_log;
	RigEditor::Main*                 m_rig_editor_instance;
	std::string                      m_scripts_base_path;
	ScriptEngineImpl*                m_impl;
};

} // namespace RigEditor

} // namespace RoR

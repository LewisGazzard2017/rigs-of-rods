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
	@date   07/2014
	@author Petr Ohlidal
*/

#pragma once

#include "RoRPrerequisites.h"
#include "Singleton.h"

#include "scriptdictionary/scriptdictionary.h"
#include "scriptbuilder/scriptbuilder.h"

namespace RoR
{

namespace RigEditor
{

// Forward decl.
class Main;

class ScriptEngine
{
public:
    ScriptEngine();
    ~ScriptEngine();
    /// @return 0 on success
    int Init();
    /// @return True on successful execution
    bool EnterRigEditor();

    void ShutDown();

    // ===== Script interface =====

    RigEditor::Main* GetRigEditorInstance();
    void             LogMessage(std::string & msg);

    // === END Script interface ===

protected:

    void MessageCallback(const AngelScript::asSMessageInfo *msg);
    
    /// @return 0 on success or AngelScript error code
    int RegisterSystemInterface();

    /// Loads "Main.as" script and resolves "#include" dependencies.
    /// @return 0 on success or AngelScript error code
    int LoadScripts();

    AngelScript::asIScriptEngine*    m_engine;
	AngelScript::asIScriptContext*   m_context;
    AngelScript::asIScriptFunction*  m_main_function;
    Ogre::Log*                       m_log;
    RigEditor::Main*                 m_rig_editor_instance;   
    std::string                      m_scripts_base_path;           
};

} // namespace RigEditor

} // namespace RoR

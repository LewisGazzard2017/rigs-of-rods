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

#include "angelscript.h"

namespace RoR
{

using namespace AngelScript;

class AngelScriptSetupHelper
{
public:
	class RegistrationException: public std::runtime_error
	{
	public:
		RegistrationException(const char* msg):
			std::runtime_error(msg)
			{}

		RegistrationException(std::string msg):
			std::runtime_error(msg)
			{}
	};

	AngelScriptSetupHelper(Ogre::Log* log, AngelScript::asIScriptEngine* engine);

	void RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv);

	void RegisterObjectType     (const char *obj, int byteSize, asDWORD flags);
	void RegisterObjectProperty (const char *obj, const char *declaration, int byteOffset);
	void RegisterObjectMethod   (const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv);
	void RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv);

protected:
	Ogre::Log*                       m_log;
	AngelScript::asIScriptEngine*    m_engine;
};

} // namespace RoR

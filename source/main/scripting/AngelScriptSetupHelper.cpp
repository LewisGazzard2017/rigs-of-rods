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

#include "AngelScriptSetupHelper.h"

using namespace RoR;
using namespace AngelScript;

AngelScriptSetupHelper::AngelScriptSetupHelper(Ogre::Log* log, AngelScript::asIScriptEngine* engine):
	m_log(log),
	m_engine(engine)
{

}

void AngelScriptSetupHelper::RegisterObjectType(const char *obj, int byteSize, asDWORD flags)
{
	int result = m_engine->RegisterObjectType(obj, byteSize, flags);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterObjectType("<<obj<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterObjectProperty(const char *obj, const char *declaration, int byteOffset)
{
	int result = m_engine->RegisterObjectProperty(obj, declaration, byteOffset);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterObjectProperty("<<obj<<", "<<declaration<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterObjectMethod(const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	int result = m_engine->RegisterObjectMethod(obj, declaration, funcPointer, callConv);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterObjectMethod("<<obj<<", "<<declaration<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	int result = m_engine->RegisterObjectBehaviour(obj, behaviour, declaration, funcPointer, callConv);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterObjectBehaviour("<<obj<<", "<<declaration<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv)
{
	int result = m_engine->RegisterGlobalFunction(declaration, funcPointer, callConv);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterGlobalFunction("<<declaration<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

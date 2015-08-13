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

#include <memory>

using namespace RoR;
using namespace AngelScript;

AngelScriptSetupHelper::AngelScriptSetupHelper(Ogre::Log* log, AngelScript::asIScriptEngine* engine):
	m_log(log),
	m_engine(engine)
{

}

const char* AngelScriptSetupHelper::ErrorCodeToString_Engine_RegisterObjectBehaviour(int err_code)
{
	switch (err_code)
	{
		case asWRONG_CONFIG_GROUP			: return "asWRONG_CONFIG_GROUP		The object type was registered in a different configuration group.		";
		case asINVALID_ARG					: return "asINVALID_ARG				obj is not set, or a global behaviour is given in behaviour.			";
		case asWRONG_CALLING_CONV			: return "asWRONG_CALLING_CONV		The function's calling convention isn't compatible with callConv.		";
		case asNOT_SUPPORTED					: return "asNOT_SUPPORTED				The calling convention or the behaviour signature is not supported.	";
		case asINVALID_TYPE					: return "asINVALID_TYPE				The obj parameter is not a valid object name.						";
		case asINVALID_DECLARATION			: return "asINVALID_DECLARATION		The declaration is invalid.												";
		case asILLEGAL_BEHAVIOUR_FOR_TYPE	: return "asILLEGAL_BEHAVIOUR_FOR_TYPEThe behaviour is not allowed for this type.							";
		case asALREADY_REGISTERED			: return "asALREADY_REGISTERED		The behaviour is already registered with the same signature.            ";
	}
	return "Unknown error code";
}

const char* AngelScriptSetupHelper::RegisterObjectMethod_ReturnCodeToString(int ret_code)
{
	switch(ret_code)
	{
		case asWRONG_CONFIG_GROUP:  return "asWRONG_CONFIG_GROUP: The object type was registered in a different configuration group.";
		case asNOT_SUPPORTED:       return "asNOT_SUPPORTED: The calling convention is not supported.";
		case asINVALID_TYPE:        return "asINVALID_TYPE: The obj parameter is not a valid object name.";
		case asINVALID_DECLARATION: return "asINVALID_DECLARATION: The declaration is invalid.";
		case asNAME_TAKEN:          return "asNAME_TAKEN: The name conflicts with other members.";
		case asWRONG_CALLING_CONV:  return "asWRONG_CALLING_CONV: The function's calling convention isn't compatible with callConv.";
		default:;
	}
	return "Unknown error code";
}

const char* AngelScriptSetupHelper::RegisterObjectProperty_ReturnCodeToString(int ret_code)
{
	switch (ret_code)
	{
		case asWRONG_CONFIG_GROUP:  return "asWRONG_CONFIG_GROUP: The object type was registered in a different configuration group.";
		case asINVALID_OBJECT:      return "asINVALID_OBJECT: The obj does not specify an object type.";
		case asINVALID_TYPE:        return "asINVALID_TYPE: The obj parameter has invalid syntax.";
		case asINVALID_DECLARATION: return "asINVALID_DECLARATION: The declaration is invalid.";
		case asNAME_TAKEN:          return "asNAME_TAKEN: The name conflicts with other members.";
		default:;
	}
	return "Unknown error code";
}

void AngelScriptSetupHelper::RegisterObjectType(const char *obj, int byteSize, asDWORD flags)
{
	int result = m_engine->RegisterObjectType(obj, byteSize, flags);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterObjectType("<<obj<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterObjectProperty(const char *obj, const char *declaration, int byteOffset)
{
	int result = m_engine->RegisterObjectProperty(obj, declaration, byteOffset);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterObjectProperty("<<obj<<", "<<declaration<<") failed, return code: " 
			<< this->RegisterObjectProperty_ReturnCodeToString(result);
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
		msg << "RegisterObjectMethod("<<obj<<", "<<declaration<<") failed, return code: " 
			<< this->RegisterObjectMethod_ReturnCodeToString(result);
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
		msg << "RegisterObjectBehaviour("<<obj<<", "<<declaration<<") failed, return code: " 
			<< this->ErrorCodeToString_Engine_RegisterObjectBehaviour(result);
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

void AngelScriptSetupHelper::RegisterInterface(const char* interface_name)
{
	int result = m_engine->RegisterInterface(interface_name);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterInterface("<<interface_name<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterInterfaceMethod(const char* interface_name, const char* method_decl)
{
	int result = m_engine->RegisterInterfaceMethod(interface_name, method_decl);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterInterfaceMethod("<<interface_name<<", "<<method_decl<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterFuncdef(const char* funcdef)
{
	int result = m_engine->RegisterFuncdef(funcdef);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterFuncdef("<<funcdef<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterEnum           (const char* enum_name)
{
	int result = m_engine->RegisterEnum(enum_name);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterEnum("<<enum_name<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

void AngelScriptSetupHelper::RegisterEnumValue      (const char* enum_name, const char* field_name, int field_value)
{
	int result = m_engine->RegisterEnumValue(enum_name, field_name, field_value);
	if (result < 0)
	{
		std::stringstream msg;
		msg << "RegisterEnumValue("<<enum_name<<", "<<field_name<<", "<<field_value<<") failed, return code: " << result;
		m_log->logMessage(msg.str());
		throw RegistrationException(msg.str());
	}
}

// Ctor
AngelScriptSetupHelper::EnumRegistrationProxy::EnumRegistrationProxy(AngelScriptSetupHelper* A, const char* enum_name)
{
	m_setup_helper = A;
	A->RegisterEnum(enum_name);
	m_enum_name = enum_name;
}

void AngelScriptSetupHelper::EnumRegistrationProxy::AddField(const char* name, int value)
{
	m_setup_helper->RegisterEnumValue(m_enum_name.c_str(), name, value);
}

AngelScriptSetupHelper::EnumRegistrationProxy AngelScriptSetupHelper::RegisterEnumWithProxy(const char* enum_name)
{
	return AngelScriptSetupHelper::EnumRegistrationProxy(this, enum_name);
}

void AngelScriptSetupHelper::ObjectRegistrationProxy::AddMethod(const char* decl, const asSFuncPtr &funcPointer, asDWORD call_conv /* = asCALL_THISCALL*/)
{
	m_setup_helper->RegisterObjectMethod(m_object_name.c_str(), decl, funcPointer, call_conv);
}

AngelScriptSetupHelper::ObjectRegistrationProxy AngelScriptSetupHelper::CreateObjectRegistrationProxy(const char* obj_name)
{
	return AngelScriptSetupHelper::ObjectRegistrationProxy(this, obj_name);
}

void AngelScriptSetupHelper::ObjectRegistrationProxy::SetupObject(int byte_size, asDWORD flags)
{
	m_setup_helper->RegisterObjectType(m_object_name.c_str(), byte_size, flags);
}
void AngelScriptSetupHelper::ObjectRegistrationProxy::AddBehavior(
	asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv /* = asCALL_THISCALL */)
{
	m_setup_helper->RegisterObjectBehaviour(m_object_name.c_str(), behaviour, declaration, funcPointer, callConv);
}

AngelScriptSetupHelper::ObjectRegistrationProxy AngelScriptSetupHelper::RegisterObjectWithProxy      (const char *obj, int byteSize, asDWORD flags)
{
	auto proxy = this->CreateObjectRegistrationProxy(obj);
	proxy.SetupObject(byteSize, flags);
	return proxy;
}

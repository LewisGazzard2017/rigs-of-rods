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

	class EnumRegistrationProxy
	{
		friend class AngelScriptSetupHelper;
		EnumRegistrationProxy(AngelScriptSetupHelper* A, const char* enum_name);
	public:
		void AddField(const char* name, int value);
	protected:	
		AngelScriptSetupHelper* m_setup_helper;
		std::string             m_enum_name;
	};

	class ObjectRegistrationProxy
	{
		friend class AngelScriptSetupHelper;
		ObjectRegistrationProxy(AngelScriptSetupHelper* A, const char* obj_name):
			m_setup_helper(A),
			m_object_name(obj_name)
		{}
	public:
		void AddMethod(const char* decl, const asSFuncPtr &func_ptr, asDWORD call_conv = asCALL_THISCALL);
	protected:	
		AngelScriptSetupHelper* m_setup_helper;
		std::string             m_object_name;
	};


	AngelScriptSetupHelper(Ogre::Log* log, AngelScript::asIScriptEngine* engine);

	template<typename T> void RegisterInheritedSharedPtr(const char* object_name, const char* T_name)
	{
		this->RegisterObjectType(object_name, sizeof(T), asOBJ_VALUE);

		char declaration[1000];
		sprintf(declaration, "%s@ Get()", T_name);
		this->RegisterObjectMethod(object_name, declaration, asMETHOD(T, AS_Get), asCALL_THISCALL);
	}

	template<typename T> void RegisterInheritedStdVector(const char* object_name, const char* T_name)
	{
		this->RegisterObjectType(object_name, sizeof(T), asOBJ_VALUE);

		char declaration[1000];
	
		sprintf(declaration, "%s At(uint32 i)", T_name);
		this->RegisterObjectMethod(object_name, declaration, asMETHOD(T, AS_At), asCALL_THISCALL);
	
		sprintf(declaration, "void PushBack(%s)", T_name);
		this->RegisterObjectMethod(object_name, declaration, asMETHOD(T, AS_PushBack), asCALL_THISCALL);

		this->RegisterObjectMethod(object_name, "void Clear()", asMETHOD(T, AS_Clear), asCALL_THISCALL);

		this->RegisterObjectMethod(object_name, "uint32 Size()", asMETHOD(T, AS_Size), asCALL_THISCALL);
	}

	// AS engine registration function wrappers, 1:1 match

	void RegisterGlobalFunction(const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv);

	void RegisterObjectType     (const char *obj, int byteSize, asDWORD flags);
	void RegisterObjectProperty (const char *obj, const char *declaration, int byteOffset);
	void RegisterObjectMethod   (const char *obj, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv);
	void RegisterObjectBehaviour(const char *obj, asEBehaviours behaviour, const char *declaration, const asSFuncPtr &funcPointer, asDWORD callConv);

	void RegisterInterface      (const char* interface_name);
	void RegisterInterfaceMethod(const char* interface_name, const char* method_decl);

	void RegisterEnum           (const char* enum_name);
	void RegisterEnumValue      (const char* enum_name, const char* field_name, int field_value);

	void RegisterFuncdef        (const char* funcdef);

	// AS engine registration utilities
	EnumRegistrationProxy   RegisterEnumWithProxy(const char* enum_name);
	ObjectRegistrationProxy CreateObjectRegistrationProxy(const char* obj_name);

	inline AngelScript::asIScriptEngine* GetEngine() { return m_engine; }

protected:
	const char* RegisterObjectMethod_ReturnCodeToString(int ret_code);
	const char* RegisterObjectProperty_ReturnCodeToString(int ret_code);

	Ogre::Log*                       m_log;
	AngelScript::asIScriptEngine*    m_engine;
};

} // namespace RoR

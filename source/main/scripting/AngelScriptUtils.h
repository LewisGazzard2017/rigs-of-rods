/*
    This source file is part of Rigs of Rods
    Copyright 2016+     Petr Ohlidal & contributors

    For more information, see http://www.rigsofrods.org/

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

#pragma once

#include <angelscript.h>
#include <string>
#include <sstream>

// Helpers
const char* AsRetCodeToString(int);
const char* AsMsgTypeToString(AngelScript::asEMsgType type);
bool        AsExecuteContext(AngelScript::asIScriptContext* ctx, AngelScript::asIScriptEngine* engine, std::string& err_msg);

class AsSetupHelper;

class AsObjectRegProxy
{
public:
    AsObjectRegProxy(AsSetupHelper* A, const char* obj_name, int byte_size, AngelScript::asDWORD flags);

    void AddBehavior(AngelScript::asEBehaviours behaviour, const char *declaration, const AngelScript::asSFuncPtr &funcPointer, AngelScript::asDWORD callConv = AngelScript::asCALL_THISCALL);
    void AddMethod(const char* decl, const AngelScript::asSFuncPtr &func_ptr, AngelScript::asDWORD call_conv = AngelScript::asCALL_THISCALL);
    void AddProperty(const char* decl, int byte_offset);

    AsSetupHelper* GetHelper() { return m_setup_helper; }

protected:
    AsSetupHelper* m_setup_helper;
    std::string    m_object_name;
    size_t         m_typeid;
};

class AsSetupHelper
{
public:
    AsSetupHelper(AngelScript::asIScriptEngine* engine);

    void                          ResetErrors();
    std::stringstream&            AddError();
    AngelScript::asIScriptEngine* GetEngine() { return m_engine; }
    bool                          CheckErrors() { return m_err_count != 0; }
    std::string                   GetErrors() { return m_err_messages.str(); }
    void                          RegisterGlobalFn(const char *decl, const AngelScript::asSFuncPtr &fn_ptr, AngelScript::asDWORD conv);

protected:
    AngelScript::asIScriptEngine*    m_engine;
    std::stringstream                m_err_messages;
    size_t                           m_err_count;
};

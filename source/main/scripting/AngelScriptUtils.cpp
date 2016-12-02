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

#include "AngelScriptUtils.h"

const char* AsRetCodeToString(int code)
{
    using namespace AngelScript;
    switch (code)
    {
    case asSUCCESS:                 return "Success";
    case asERROR:                   return "Failure";
    case asCONTEXT_ACTIVE:          return "The context is active";
    case asCONTEXT_NOT_FINISHED:    return "The context is not finished";
    case asCONTEXT_NOT_PREPARED:    return "The context is not prepared";
    case asINVALID_ARG:             return "Invalid argument";
    case asNO_FUNCTION:             return "The function was not found";
    case asNOT_SUPPORTED:           return "Not supported";
    case asINVALID_NAME:            return "Invalid name";
    case asNAME_TAKEN:              return "The name is already taken";
    case asINVALID_DECLARATION:     return "Invalid declaration";
    case asINVALID_OBJECT:          return "Invalid object";
    case asINVALID_TYPE:            return "Invalid type";
    case asALREADY_REGISTERED:      return "Already registered";
    case asMULTIPLE_FUNCTIONS:      return "Multiple matching functions";
    case asNO_MODULE:               return "The module was not found";
    case asNO_GLOBAL_VAR:           return "The global variable was not found";
    case asINVALID_CONFIGURATION:   return "Invalid configuration";
    case asINVALID_INTERFACE:       return "Invalid interface";
    case asWRONG_CONFIG_GROUP:      return "Wrong configuration group";
    case asCONFIG_GROUP_IS_IN_USE:  return "The configuration group is in use";
    case asWRONG_CALLING_CONV:      return "The specified calling convention doesn't match the function/method pointer";
    case asBUILD_IN_PROGRESS:       return "A build is currently in progress";
    case asCANT_BIND_ALL_FUNCTIONS:       return "All imported functions couldn't be bound";
    case asINIT_GLOBAL_VARS_FAILED:       return "The initialization of global variables failed.";
    case asILLEGAL_BEHAVIOUR_FOR_TYPE:           return "Illegal behaviour for the type";
    case asLOWER_ARRAY_DIMENSION_NOT_REGISTERED: return "The array sub type has not been registered yet";
    //case asOUT_OF_MEMORY:    return "It wasn't possible to allocate the needed memory";
    //case asMODULE_IS_IN_USE: return "The module is referred to by live objects or from the application";
    default: return "~Unknown error~";
    }
}

const char* AsMsgTypeToString(AngelScript::asEMsgType type)
{
    switch (type)
    {
    case AngelScript::asMSGTYPE_INFORMATION: return "Info";
    case AngelScript::asMSGTYPE_WARNING:     return "Warning";
    case AngelScript::asMSGTYPE_ERROR:       return "Error";
    default:                                 return "~Msg~";
    }
}

bool AsExecuteContext(AngelScript::asIScriptContext* ctx, AngelScript::asIScriptEngine* engine, std::string& err_msg)
{
    using namespace AngelScript;
    int result = ctx->Execute();

    if( result == asEXECUTION_FINISHED )
    {
        return true;
    }

    std::stringstream msg;
    if( result == asEXECUTION_ABORTED )
    {
        msg << "The execution was aborted with a call to Abort (asEXECUTION_ABORTED).";
    }
    else if( result == asEXECUTION_EXCEPTION )
    {
        msg << "The script ended with an exception." << std::endl;

        int funcId = ctx->GetExceptionFunction();
        const asIScriptFunction *func = engine->GetFunctionById(funcId);
        msg << "\tFunction: " << func->GetDeclaration()        << std::endl;
        msg << "\t  Module: " << func->GetModuleName()         << std::endl;
        msg << "\t Section: " << func->GetScriptSectionName()  << std::endl;
        msg << "\t LineNum: " << ctx->GetExceptionLineNumber() << std::endl;
        msg << "\t Message: " << ctx->GetExceptionString()     << std::endl;
    }
    else
    {
        msg << "The script ended for some unforeseen reason (result=" << result << ")." << std::endl;
    }
    err_msg = msg.str();
    return false;
}

void AsSetupHelper::ResetErrors()
{
    m_err_count = 0;
    m_err_messages.str(""); // Reset
}

std::stringstream& AsSetupHelper::AddError()
{
    ++m_err_count;
    return m_err_messages;
}

AsObjectRegProxy::AsObjectRegProxy(AsSetupHelper* A, const char* obj_name, int byte_size, AngelScript::asDWORD flags):
    m_setup_helper(A),
    m_object_name(obj_name),
    m_typeid(0)
{
    int typeid_or_error = m_setup_helper->GetEngine()->RegisterObjectType(m_object_name.c_str(), byte_size, flags);
    if (typeid_or_error < 0)
    {
        m_setup_helper->AddError() << "RegisterObjectType("<<m_object_name<<") failed, error: " << AsRetCodeToString(typeid_or_error) << std::endl;
    }
    else
    {
        m_typeid = static_cast<size_t>(typeid_or_error);
    }
}

void AsObjectRegProxy::AddBehavior(AngelScript::asEBehaviours behaviour, const char *declaration, const AngelScript::asSFuncPtr &funcPointer, AngelScript::asDWORD callConv)
{
    int id_or_error = m_setup_helper->GetEngine()->RegisterObjectBehaviour(m_object_name.c_str(), behaviour, declaration, funcPointer, callConv);
    if (id_or_error < 0)
    {
        m_setup_helper->AddError() << "RegisterObjectBehaviour("<<m_object_name<<", "<<declaration
            <<") failed, message: " << AsRetCodeToString(id_or_error) << std::endl;
    }
}

void AsObjectRegProxy::AddMethod(const char* decl, const AngelScript::asSFuncPtr &func_ptr, AngelScript::asDWORD call_conv)
{
    int id_or_error = m_setup_helper->GetEngine()->RegisterObjectMethod(m_object_name.c_str(), decl, func_ptr, call_conv);
    if (id_or_error < 0)
    {
        m_setup_helper->AddError() << "RegisterObjectMethod("<<m_object_name<<", "<<decl
            <<") failed, message: " << AsRetCodeToString(id_or_error) << std::endl;
    }
}

void AsObjectRegProxy::AddProperty(const char* decl, int byte_offset)
{
    int result = m_setup_helper->GetEngine()->RegisterObjectProperty(m_object_name.c_str(), decl, byte_offset);
    if (result < 0)
    {
        m_setup_helper->AddError() << "RegisterObjectProperty("<<m_object_name<<", "<<decl
            <<") failed, message: " << AsRetCodeToString(result) << std::endl;
    }
}

AsSetupHelper::AsSetupHelper(AngelScript::asIScriptEngine* engine):
    m_engine(engine),
    m_err_count(0)
{
}

void AsSetupHelper::RegisterGlobalFn(const char *decl, const AngelScript::asSFuncPtr &funcPointer, AngelScript::asDWORD callConv)
{
    int result = m_engine->RegisterGlobalFunction(decl, funcPointer, callConv);
    if (result < 0)
    {
        this->AddError() << "RegisterGlobalFunction("<<decl<<") failed, message: " << AsRetCodeToString(result);
    }
}


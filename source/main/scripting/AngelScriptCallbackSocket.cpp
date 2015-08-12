
#include "AngelScriptCallbackSocket.h"
#include "RigEditor_ScriptEngine.h"

#include "angelscript.h"

#include <sstream>

using namespace AngelScript;
using namespace RoR;
using namespace RigEditor; // for ScriptEngine

AngelScriptCallbackSocket::AngelScriptCallbackSocket():
		m_as_callback_object(nullptr),
		m_as_callback_method(nullptr)
	{}


void AngelScriptCallbackSocket::RegisterCallback(AngelScript::asIScriptObject* object, std::string method_name)
{
	if (object == nullptr)
	{
		throw std::runtime_error("AngelScriptCallbackSocket::RegisterCallback(): Object parameter is null");
	}
	// Find method
	AngelScript::asIScriptFunction* method = object->GetObjectType()->GetMethodByName(method_name.c_str());
	if (method == nullptr)
	{
		std::stringstream msg;
		msg << "AngelScriptCallbackSocket::RegisterCallback(): Failed to find method \""<<method_name<<"\" by name.";
		throw std::runtime_error(msg.str());
	}
	// Check that object + function match.
	if(object->GetObjectType() != method->GetObjectType())
	{
		throw std::runtime_error("AngelScriptCallbackSocket::RegisterCallback() The supplied  object type and method's object type don't match.");
	}

	m_as_callback_object = object;
	m_as_callback_method = method;
}

AngelScript::asIScriptContext* AngelScriptCallbackSocket::PrepareContext()
{
	AngelScript::asIScriptContext* ctx = m_as_callback_method->GetEngine()->CreateContext(); // Sub optimal, to be improved
	int result = ctx->Prepare(m_as_callback_method);
	if (result != 0)
	{
		std::stringstream msg;
		msg <<__FUNCTION__<< "(): Failed to Prepare() context, error code: " << ScriptEngine::ContextPrepare_ErrorCodeToString(result);
		throw std::runtime_error(msg.str());
	}
	result = ctx->SetObject(m_as_callback_object);
	if (result != 0)
	{
		std::stringstream msg;
		msg <<__FUNCTION__<< "(): Failed to SetObject() context, error code: " << result;
		throw std::runtime_error(msg.str());
	}
	return ctx;
}

void AngelScriptCallbackSocket::SetArgObject(AngelScript::asIScriptContext* ctx, int idx, void* object)
{
	int result = ctx->SetArgObject(idx, object);
	if (result != 0)
	{
		std::stringstream msg;
		msg <<__FUNCTION__<< "(): Failed to SetArgObject("<<idx<<", void*) to context, error code: " 
			<< ScriptEngine::ContextSetArg_ErrorCodeToString(result);
		throw std::runtime_error(msg.str());
	}
}

void AngelScriptCallbackSocket::SetArgInt(AngelScript::asIScriptContext* ctx, int idx, int n)
{
	int result = ctx->SetArgDWord(idx,  static_cast<asDWORD>(n));
	if (result != 0)
	{
		std::stringstream msg;
		msg <<__FUNCTION__<< "(): Failed to SetArgDWord("<<idx<<", asDWORD) to context, error code: " 
			<< ScriptEngine::ContextSetArg_ErrorCodeToString(result);
		throw std::runtime_error(msg.str());
	}	
}

void AngelScriptCallbackSocket::ExecuteContext(AngelScript::asIScriptContext* ctx)
{
	ScriptEngine::ExecuteContext(ctx, m_as_callback_method->GetEngine());	
}
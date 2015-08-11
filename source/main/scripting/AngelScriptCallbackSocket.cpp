
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


void AngelScriptCallbackSocket::RegisterCallback(AngelScript::asIScriptObject* object, AngelScript::asIScriptFunction* method)
{
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
	this->SetArgObject(ctx, 0, static_cast<void*>(m_as_callback_object));
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
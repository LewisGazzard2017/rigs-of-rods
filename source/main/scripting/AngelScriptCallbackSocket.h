
#pragma once

#include <string>

// Forward decl.
namespace AngelScript
{
	class asIScriptObject;
	class asIScriptFunction;
	class asIScriptContext;
}

namespace RoR
{

class AngelScriptCallbackSocket
{
public:
	AngelScriptCallbackSocket();

	bool IsBound()
	{
		return ! (m_as_callback_method == nullptr || m_as_callback_object == nullptr);
	}

	void RegisterCallback(AngelScript::asIScriptObject* object, std::string method_name);

	AngelScript::asIScriptContext* PrepareContext();

	void   SetArgObject(  AngelScript::asIScriptContext* ctx, int idx, void*    object);
	void   SetArgInt(     AngelScript::asIScriptContext* ctx, int idx, int      n);
	void   SetArgBool(    AngelScript::asIScriptContext* ctx, int idx, bool     b);
	void   ExecuteContext(AngelScript::asIScriptContext* ctx);
protected:

	AngelScript::asIScriptObject*   m_as_callback_object;
	AngelScript::asIScriptFunction* m_as_callback_method;
};

} // namespace RoR

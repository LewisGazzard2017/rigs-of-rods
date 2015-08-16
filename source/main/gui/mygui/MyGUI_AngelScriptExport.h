
#pragma once

#include "AngelScriptSetupHelper.h"

#include "MyGUI.h"

struct AngelScriptNoRefCount
{
	void AS_AddRef() {}
	void AS_ReleaseRef() {}
};

struct MyGUI_AngelScriptExport
{
	template<typename T>
	static RoR::AngelScriptSetupHelper::ObjectRegistrationProxy 
	RegisterRefObject(RoR::AngelScriptSetupHelper* A, const char* obj_name);

	static void Export(RoR::AngelScriptSetupHelper* A);
};


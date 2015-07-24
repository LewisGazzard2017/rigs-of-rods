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

#include "RigEditor_ScriptEngine.h"

//#include "MainThread.h"
#include "Application.h"
#include "ContentManager.h"
#include "Settings.h"
#include "PlatformUtils.h"
#include "RigEditor_Config.h"
#include "RigEditor_Main.h"
#include "RoRPrerequisites.h"

// AS addons start
#include "scriptstdstring/scriptstdstring.h"
#include "scriptmath/scriptmath.h"
#include "contextmgr/contextmgr.h"
#include "scriptany/scriptany.h"
#include "scriptarray/scriptarray.h"
#include "scripthelper/scripthelper.h"
#include "scriptstring/scriptstring.h"
// AS addons end

// WORKAROUND for RoR dependency pack 26
// Cloned from as_scriptbuilder.cpp
#include <stdio.h>
#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#include <direct.h>
#endif
#ifdef _WIN32_WCE
#include <windows.h> // For GetModuleFileName
#endif


using namespace RoR;
using namespace RigEditor;
using namespace AngelScript;
using namespace std;

// Temporary hack for exporting to AngelScript
RigEditor::ScriptEngine* global_rig_editor_script_engine_instance;
RigEditor::Main* GlobalGetRigEditorInstance()
{
    return global_rig_editor_script_engine_instance->GetRigEditorInstance();
}

void GlobalLogMessage(std::string msg)
{
    global_rig_editor_script_engine_instance->LogMessage(msg);
}

// WORKAROUND
// =====================================================================
// Cloned from as_scriptbuilder.cpp
// RoR has LoadScriptSection() modified to "virtual int f() = 0;"
// This subclass defines the function using a clone of itself :D
class ScriptBuilderWorkaround: public CScriptBuilder
{
protected:
    int LoadScriptSection(const char *filename);    
    const char *GetCurrentDirectory(char *buf, size_t size);
};

int ScriptBuilderWorkaround::LoadScriptSection(const char *filename)
{
	// Open the script file
	string scriptFile = filename;
#if _MSC_VER >= 1500
	FILE *f = 0;
	fopen_s(&f, scriptFile.c_str(), "rb");
#else
	FILE *f = fopen(scriptFile.c_str(), "rb");
#endif
	if( f == 0 )
	{
		// Write a message to the engine's message callback
		char buf[256];
		string msg = "Failed to open script file '" + string(GetCurrentDirectory(buf, 256)) + "\\" + scriptFile + "'";
		engine->WriteMessage(filename, 0, 0, asMSGTYPE_ERROR, msg.c_str());

		// TODO: Write the file where this one was included from

		return -1;
	}
	
	// Determine size of the file
	fseek(f, 0, SEEK_END);
	int len = ftell(f);
	fseek(f, 0, SEEK_SET);

	// On Win32 it is possible to do the following instead
	// int len = _filelength(_fileno(f));

	// Read the entire file
	string code;
	code.resize(len);
	size_t c = fread(&code[0], len, 1, f);

	fclose(f);

	if( c == 0 ) 
	{
		// Write a message to the engine's message callback
		char buf[256];
		string msg = "Failed to load script file '" + string(GetCurrentDirectory(buf, 256)) + scriptFile + "'";
		engine->WriteMessage(filename, 0, 0, asMSGTYPE_ERROR, msg.c_str());
		return -1;
	}

	return ProcessScriptSection(code.c_str(), filename);
}

const char *ScriptBuilderWorkaround::GetCurrentDirectory(char *buf, size_t size)
{
#ifdef _MSC_VER
#ifdef _WIN32_WCE
    static TCHAR apppath[MAX_PATH] = TEXT("");
    if (!apppath[0])
    {
        GetModuleFileName(NULL, apppath, MAX_PATH);

        
        int appLen = _tcslen(apppath);

        // Look for the last backslash in the path, which would be the end
        // of the path itself and the start of the filename.  We only want
        // the path part of the exe's full-path filename
        // Safety is that we make sure not to walk off the front of the 
        // array (in case the path is nothing more than a filename)
        while (appLen > 1)
        {
            if (apppath[appLen-1] == TEXT('\\'))
                break;
            appLen--;
        }

        // Terminate the string after the trailing backslash
        apppath[appLen] = TEXT('\0');
    }
#ifdef _UNICODE
    wcstombs(buf, apppath, min(size, wcslen(apppath)*sizeof(wchar_t)));
#else
    memcpy(buf, apppath, min(size, strlen(apppath)));
#endif

    return buf;
#else
	return _getcwd(buf, (int)size);
#endif
#elif defined(__APPLE__)
	return getcwd(buf, size);
#else
	return "";
#endif
}

// END OF WORKAROUND =========================================================

ScriptEngine::ScriptEngine():
    m_engine(nullptr),
    m_context(nullptr),
    m_rig_editor_instance(nullptr),
    m_main_function(nullptr),
    m_log(nullptr)
{
    global_rig_editor_script_engine_instance = this; // Hack for interfacing with AngelScript through global functions
}

ScriptEngine::~ScriptEngine()
{
	this->ShutDown();
}

int ScriptEngine::LoadScripts()
{

	// The builder is a helper class that will load the script file, 
	// search for #include directives, and load any included files as 
	// well.
	ScriptBuilderWorkaround builder;

	// Build the script. If there are any compiler messages they will
	// be written to the message stream that we set right after creating the 
	// script engine. If there are no errors, and no warnings, nothing will
	// be written to the stream.
	int result = builder.StartNewModule(m_engine, 0);
	if( result < 0 )
	{
        std::stringstream msg;
        msg << __FUNCTION__ << "Failed to start new AngelScript module, result:" << result;
        m_log->logMessage(msg.str());
        return result;
	}
    std::string main_script_path = SSETTING("RigEditor Scripts Path", "") + PlatformUtils::DIRECTORY_SEPARATOR + "Main.as";
	result = builder.AddSectionFromFile(main_script_path.c_str());
	if( result < 0 )
	{
		std::stringstream msg;
        msg << __FUNCTION__ << "Failed to load main script Main.as, result:" << result << ", path: " << main_script_path;
        m_log->logMessage(msg.str());
        return result;
	}
	result = builder.BuildModule();
	if( result < 0 )
	{
		std::stringstream msg;
        msg << __FUNCTION__ << "Failed to build the AngelScript module, result:" << result;
        m_log->logMessage(msg.str());
		return result;
	}
	
	// The engine doesn't keep a copy of the script sections after Build() has
	// returned. So if the script needs to be recompiled, then all the script
	// sections must be added again.

	// If we want to have several scripts executing at different times but 
	// that have no direct relation with each other, then we can compile them
	// into separate script modules. Each module use their own namespace and 
	// scope, so function names, and global variables will not conflict with
	// each other.

    return 0;
}

int ScriptEngine::Init()
{
    m_log = Ogre::LogManager::getSingleton().createLog(SSETTING("Log Path", "")+"/RigEditor.log", false);

    // Create the script engine
    m_engine = AngelScript::asCreateScriptEngine(ANGELSCRIPT_VERSION);
    if (m_engine == nullptr)
    {
        m_log->logMessage("ScriptEngine::Init(): failed to create script engine.");
        return 1;
    }
    
    int result = 0;

    // Set the message callback to receive information on errors in human readable form.
    result = m_engine->SetMessageCallback(AngelScript::asMETHOD(RigEditor::ScriptEngine,MessageCallback), this, AngelScript::asCALL_THISCALL);
    if(result < 0)
    {
        std::stringstream msg;
        msg << __FUNCTION__ << "Failed to set MessageCallback function, result:" << result;
        m_log->logMessage(msg.str());
        return result;
    }

    // AngelScript doesn't have a built-in string type, as there is no definite standard
	// string type for C++ applications. Every developer is free to register it's own string type.
	// The SDK do however provide a standard add-on for registering a string type, so it's not
	// necessary to register your own string type if you don't want to.
	AngelScript::RegisterScriptArray     (m_engine, true);
	AngelScript::RegisterStdString       (m_engine);
	AngelScript::RegisterStdStringUtils  (m_engine);
	AngelScript::RegisterScriptMath      (m_engine);
	AngelScript::RegisterScriptAny       (m_engine);
	AngelScript::RegisterScriptDictionary(m_engine);

    result = this->RegisterSystemInterface();
    if (result != 0)
    {
        std::stringstream msg;
        msg << __FUNCTION__ << "Failed to RegisterSystemInterface(), result:" << result;
        m_log->logMessage(msg.str());
        m_engine->Release();
        m_engine = nullptr;
        return result;
    }
        
    result = this->LoadScripts();
    if (result != 0)
    {
        std::stringstream msg;
        msg << __FUNCTION__ << "Failed to LoadScripts(), result:" << result;
        m_log->logMessage(msg.str());
        m_engine->Release();
        m_engine = nullptr;
        return result;
    }

    // Create a context that will execute the script.
	m_context = m_engine->CreateContext();
	if( m_context == nullptr ) 
	{
		std::stringstream msg;
        msg << __FUNCTION__ << "Failed to create AngelScript context, result:" << result;
        m_log->logMessage(msg.str());
		m_engine->Release();
        m_engine = nullptr;
		return result;
	}

    // Find the function for the function we want to execute.
	m_main_function = m_engine->GetModule(0)->GetFunctionByDecl("void Main()");
	if( m_main_function == nullptr )
	{
		std::stringstream msg;
        msg << __FUNCTION__ << "Failed to find function 'void Main()' in the loaded scripts.";
        m_log->logMessage(msg.str());
		m_context->Release();
        m_context = nullptr;
		m_engine->Release();
        m_engine = nullptr;
		return 1;
	}

    return 0;
}

int ScriptEngine::RegisterSystemInterface()
{
    using namespace AngelScript;
    int result = 0;
        
    //result = m_engine->SetDefaultNamespace("ROR_SYSTEM"); // MUST UPDATE DEPS 
    //if(result < 0) return result;

    result = m_engine->RegisterGlobalFunction("void LogMessage(string msg)",
        AngelScript::asFUNCTION(GlobalLogMessage), AngelScript::asCALL_CDECL);
    if(result < 0) return result;

    // RigEditor::Main

    result = m_engine->RegisterObjectType("RigEditorCore_UGLY", 0, asOBJ_REF /*| asOBJ_NOCOUNT*/); // MUST UPDATE DEPS
    if(result < 0) return result;

    result = m_engine->RegisterObjectBehaviour("RigEditorCore_UGLY", asBEHAVE_ADDREF,
        "void f()", asMETHOD(RigEditor::Main, AngelscriptRefCountAdd), asCALL_THISCALL);
    if(result < 0) return result;
    result = m_engine->RegisterObjectBehaviour("RigEditorCore_UGLY", asBEHAVE_RELEASE,
        "void f()", asMETHOD(RigEditor::Main, AngelscriptRefCountRelease), asCALL_THISCALL);
    if(result < 0) return result;

    result = m_engine->RegisterGlobalFunction("RigEditorCore_UGLY@ GetRigEditorInstance_UGLY()",
        AngelScript::asFUNCTION(GlobalGetRigEditorInstance), AngelScript::asCALL_CDECL);
    if(result < 0) return result;

    result = m_engine->RegisterObjectMethod("RigEditorCore_UGLY", "void OnEnter_SetupInput_UGLY()", 
        asMETHOD(RigEditor::Main,OnEnter_SetupInput_UGLY), asCALL_THISCALL);
    if(result < 0) return result;

    result = m_engine->RegisterObjectMethod("RigEditorCore_UGLY", "void OnEnter_SetupCameraAndViewport_UGLY()", 
        asMETHOD(RigEditor::Main,OnEnter_SetupCameraAndViewport_UGLY), asCALL_THISCALL);
    if(result < 0) return result;

    result = m_engine->RegisterObjectMethod("RigEditorCore_UGLY", "void OnEnter_InitializeOrRestoreGui_UGLY()", 
        asMETHOD(RigEditor::Main, OnEnter_InitializeOrRestoreGui_UGLY), asCALL_THISCALL);
    if(result < 0) return result;

    result = m_engine->RegisterObjectMethod("RigEditorCore_UGLY", "void OnEnter_RunMainLoop_UGLY()", 
        asMETHOD(RigEditor::Main,OnEnter_RunMainLoop_UGLY), asCALL_THISCALL);
    if(result < 0) return result;

    //m_engine->RegisterObjectMethod("mytype", "void ClassMethod()", asMETHOD(MyClass,ClassMethod), asCALL_THISCALL); assert( r >= 0 );

    //result = m_engine->SetDefaultNamespace("");  // MUST UPDATE DEPS
    if(result < 0) return result;

    return 0;
}

RigEditor::Main* ScriptEngine::GetRigEditorInstance()
{
    if (m_rig_editor_instance != nullptr)
    {
        return m_rig_editor_instance;
    }
    RoR::Application::GetContentManager()->AddResourcePack(ContentManager::ResourcePack::RIG_EDITOR);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("RigEditor");

    RigEditor::Config* rig_editor_config = new RigEditor::Config(SSETTING("Config Root", "") + "rig_editor.cfg");
    assert(rig_editor_config != nullptr);
    m_rig_editor_instance = new RigEditor::Main(rig_editor_config);
    
    return m_rig_editor_instance;
}

void ScriptEngine::MessageCallback(const AngelScript::asSMessageInfo *msg)
{
	const char *type = "Error";
	if ( msg->type == AngelScript::asMSGTYPE_INFORMATION )
		type = "Info";
	else if ( msg->type == AngelScript::asMSGTYPE_WARNING )
		type = "Warning";

	char tmp[1024]="";
	sprintf(tmp, "%s (%d, %d): %s = %s", msg->section, msg->row, msg->col, type, msg->message);
	m_log->logMessage(tmp);
}

bool ScriptEngine::EnterRigEditor()
{
    // Prepare the script context with the function we wish to execute. Prepare()
	// must be called on the context before each new script function that will be
	// executed.
	int result = m_context->Prepare(m_main_function);
	if( result < 0 ) 
	{
		std::stringstream msg;
        msg << __FUNCTION__ << "Failed to prepare AngelScript context, result: " << result;
        m_log->logMessage(msg.str());
		return false;
	}

    // Execute the Main() function
	m_log->logMessage("Executing the script's Main() function");
    m_log->logMessage("==================================================");
	result = m_context->Execute();
    m_log->logMessage("==================================================");
	m_log->logMessage("The script's Main() function has finished");

    std::stringstream msg;
    bool success = true;
	if( result != asEXECUTION_FINISHED )
	{
        success = false;
		// The execution didn't finish as we had planned. Determine why.
		if( result == asEXECUTION_ABORTED )
        {
            msg << "The execution was aborted with a call to Abort (asEXECUTION_ABORTED).";
        }
		else if( result == asEXECUTION_EXCEPTION )
		{
			msg << "The script ended with an exception." << endl;

			// Write some information about the script exception
            
			int funcId = m_context->GetExceptionFunction();
            const asIScriptFunction *func = m_engine->GetFunctionById(funcId);
			msg << "\tfunc: " << func->GetDeclaration() << endl;
			msg << "\tmodl: " << func->GetModuleName() << endl;
			msg << "\tsect: " << func->GetScriptSectionName() << endl;
			msg << "\tline: " << m_context->GetExceptionLineNumber() << endl;
			msg << "\tdesc: " << m_context->GetExceptionString() << endl;
		}
		else
        {
			msg << "The script ended for some unforeseen reason (result=" << result << ")." << endl;
        }
	}
	else
	{
		msg << "The script finished successfully, return code: " << result;
	}
    m_log->logMessage(msg.str());

	return success;
}

void ScriptEngine::ShutDown()
{
    if (m_context != nullptr && m_engine != nullptr)
    {
        m_main_function = nullptr;

	    m_context->Release();
        m_context = nullptr;

        // TODO: UPDATE DEPS AND USE: ShutDownAndRelease()
	    m_engine->Release();
        m_engine = nullptr;
    }
}

void ScriptEngine::LogMessage(std::string & msg)
{
    m_log->logMessage(msg);
}

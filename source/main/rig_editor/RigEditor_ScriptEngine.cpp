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
//#include "AngelScriptSetupHelper.h"
///////////////////////////////////////
#include "Application.h"
#include "ContentManager.h"
#include "Settings.h"
#include "MainThread.h"
//#include "MyGUI_AngelScriptExport.h"
#include "OgreSubsystem.h"
#include "PlatformUtils.h"
#include "RigEditor_Config.h"
#include "RigEditor_Main.h"
#include "RoRPrerequisites.h"

#include <OgreRoot.h>
#include <OgreRenderWindow.h>

#include <boost/python/detail/wrap_python.hpp> // Replaces #include <Python.h>, recommended in Boost docs
#include <boost/python.hpp>

using namespace RoR;
using namespace RigEditor;
using namespace std;

// Temporary hack for exporting to AngelScript
// Will be changed when AngelScript is updated.
RigEditor::ScriptEngine* global_rig_editor_script_engine_instance;
RigEditor::Main* AS_GetRigEditorInstance()
{
    return global_rig_editor_script_engine_instance->GetRigEditorInstance();
}

void AS_GlobalLogMessage(std::string msg)
{
    global_rig_editor_script_engine_instance->LogMessage(msg);
}

bool AS_IsRoRApplicationWindowClosed()
{
	return RoR::Application::GetOgreSubsystem()->GetRenderWindow()->isClosed();
}

void AS_RequestRoRShutdown()
{
	RoR::Application::GetMainThreadLogic()->RequestShutdown();
}

void AS_RenderFrameAndUpdateWindow()
{
	RoR::Application::GetOgreSubsystem()->GetOgreRoot()->renderOneFrame();
	Ogre::RenderWindow* rw = RoR::Application::GetOgreSubsystem()->GetRenderWindow();
	if (!rw->isActive() && rw->isVisible())
	{
		rw->update(); // update even when in background !
	}
}

std::string AS_SYS_GetStringSetting(std::string key, std::string default_val)
{
	return std::string(SSETTING(key, default_val));
}

std::string AS_SYS_LoadRigEditorResourceAsString(std::string filename)
{
	Ogre::DataStreamPtr helpfile_stream = Ogre::ResourceGroupManager::getSingleton().openResource(
		filename, // TODO: Localization
		RoR::ContentManager::ResourcePack::RIG_EDITOR.resource_group_name,
		false);

	if (!helpfile_stream.isNull())
	{
		return helpfile_stream->getAsString();
	}
	return std::string();
}


ScriptEngine::ScriptEngine():
    
    m_rig_editor_instance(nullptr),
    
    m_log(nullptr)
{
    global_rig_editor_script_engine_instance = this; // Hack for interfacing with AngelScript through global functions
}

ScriptEngine::~ScriptEngine()
{
	this->ShutDown();
}
#if 0
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
#endif

void ScriptEngine::Init()
{
    m_log = Ogre::LogManager::getSingleton().createLog(SSETTING("Log Path", "")+"/RigEditorScriptEngine.log", false);

	Py_Initialize();  // start the interpreter and create the __main__ module.
	m_log->logMessage("Py_Initialize() DONE");
}

const char* PythonUnicodeKindToString(int py_kind)
{
	switch (py_kind)
	{
		case PyUnicode_WCHAR_KIND: return "PyUnicode_WCHAR_KIND";
		case PyUnicode_1BYTE_KIND: return "PyUnicode_1BYTE_KIND";
		case PyUnicode_2BYTE_KIND: return "PyUnicode_2BYTE_KIND";
		case PyUnicode_4BYTE_KIND: return "PyUnicode_4BYTE_KIND";
	}
}


#if 0
    // Set the message callback to receive information on errors in human readable form.
    result = m_engine->SetMessageCallback(AngelScript::asMETHOD(RigEditor::ScriptEngine,MessageCallback), this, AngelScript::asCALL_THISCALL);
    if(result < 0)
    {
        std::stringstream msg;
        msg << __FUNCTION__ << "(): Failed to set MessageCallback function, result:" << result;
        m_log->logMessage(msg.str());
        return result;
    }

    result = this->RegisterSystemInterface();
    if (result != 0)
    {
        std::stringstream msg;
        msg << __FUNCTION__ << "(): Failed to RegisterSystemInterface(), result:" << result;
        m_log->logMessage(msg.str());
        m_engine->Release();
        m_engine = nullptr;
        return result;
    }
        
    result = this->LoadScripts();
    if (result != 0)
    {
        std::stringstream msg;
        msg << __FUNCTION__ << "(): Failed to LoadScripts(), result:" << result;
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
        msg << __FUNCTION__ << "(): Failed to create AngelScript context, result:" << result;
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
        msg << __FUNCTION__ << "(): Failed to find function 'void Main()' in the loaded scripts.";
        m_log->logMessage(msg.str());
		m_context->Release();
        m_context = nullptr;
		m_engine->Release();
        m_engine = nullptr;
		return 1;
	}

#endif


#if 0
int ScriptEngine::RegisterSystemInterface()
{
    using namespace AngelScript;
	AngelScriptSetupHelper A(m_log, m_engine);
	try
	{
		A.RegisterGlobalFunction("void LogMessage(string msg)", asFUNCTION(AS_GlobalLogMessage), asCALL_CDECL);

		// RigEditor::Main
		A.RegisterObjectType     ("RigEditorCore_UGLY", 0, asOBJ_REF);
		A.RegisterObjectBehaviour("RigEditorCore_UGLY", asBEHAVE_ADDREF,  "void f()",     asMETHOD(RigEditor::Main, AS_RefCountAdd),     asCALL_THISCALL);
		A.RegisterObjectBehaviour("RigEditorCore_UGLY", asBEHAVE_RELEASE, "void f()",     asMETHOD(RigEditor::Main, AS_RefCountRelease), asCALL_THISCALL);

		auto class_main_proxy = A.CreateObjectRegistrationProxy("RigEditorCore_UGLY");
		class_main_proxy.AddMethod( "void OnEnter_SetupInput_UGLY()",                    asMETHOD(Main, AS_OnEnter_SetupInput_UGLY)            );
		class_main_proxy.AddMethod( "void OnEnter_SetupCameraAndViewport_UGLY()",        asMETHOD(Main, AS_OnEnter_SetupCameraAndViewport_UGLY));
		class_main_proxy.AddMethod( "void OnEnter_InitializeOrRestoreGui_UGLY()",        asMETHOD(Main, AS_OnEnter_InitializeOrRestoreGui_UGLY));
								    											         
		class_main_proxy.AddMethod( "void UpdateMainLoop_UGLY()",                        asMETHOD(Main, AS_UpdateMainLoop_UGLY)                );
		class_main_proxy.AddMethod( "bool WasExitLoopRequested_UGLY()",                  asMETHOD(Main, AS_WasExitLoopRequested_UGLY)          );
								    											         
		class_main_proxy.AddMethod( "void OnExit_HideGui_UGLY()",                        asMETHOD(Main, AS_OnExit_HideGui_UGLY)                );
		class_main_proxy.AddMethod( "void OnExit_ClearExitRequest_UGLY()",               asMETHOD(Main, AS_OnExit_ClearExitRequest_UGLY)       );
																					  
		class_main_proxy.AddMethod( "void LoadRigDefFile_UGLY(string folder, string file)", asMETHOD(Main, AS_LoadRigDefFile));
		class_main_proxy.AddMethod( "void SaveRigDefFile_UGLY(string folder, string file)", asMETHOD(Main, AS_SaveRigDefFile));

		class_main_proxy.AddMethod( "void HandleCommandCloseCurrentRig_UGLY()",                    asMETHOD(Main, AS_HandleCommandCloseCurrentRig_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandCreateNewEmptyRig_UGLY()",                  asMETHOD(Main, AS_HandleCommandCreateNewEmptyRig_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandCurrentRigDeleteSelectedNodes_UGLY()",      asMETHOD(Main, AS_HandleCommandCurrentRigDeleteSelectedNodes_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandCurrentRigDeleteSelectedBeams_UGLY()",      asMETHOD(Main, AS_HandleCommandCurrentRigDeleteSelectedBeams_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandQuitRigEditor_UGLY()",                      asMETHOD(Main, AS_HandleCommandQuitRigEditor_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandShowRigPropertiesWindow_UGLY()",            asMETHOD(Main, AS_HandleCommandShowRigPropertiesWindow_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandSaveContentOfRigPropertiesWindow_UGLY()",   asMETHOD(Main, AS_HandleCommandSaveContentOfRigPropertiesWindow_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandShowLandVehiclePropertiesWindow_UGLY()",    asMETHOD(Main, AS_HandleCommandShowLandVehiclePropertiesWindow_UGLY));
		class_main_proxy.AddMethod( "void HandleCommandSaveLandVehiclePropertiesWindowData_UGLY()",asMETHOD(Main, AS_HandleCommandSaveLandVehiclePropertiesWindowData_UGLY));

		// Command callback interface
		auto enum_cmd_proxy = A.RegisterEnumWithProxy  ("RigEditorUserCommand_UGLY");
		enum_cmd_proxy.AddField("USER_COMMAND_SHOW_DIALOG_OPEN_RIG_FILE"               , (int)IMain::USER_COMMAND_SHOW_DIALOG_OPEN_RIG_FILE);
		enum_cmd_proxy.AddField("USER_COMMAND_SHOW_DIALOG_SAVE_RIG_FILE_AS"			   , (int)IMain::USER_COMMAND_SHOW_DIALOG_SAVE_RIG_FILE_AS);
		enum_cmd_proxy.AddField("USER_COMMAND_SAVE_RIG_FILE"						   , (int)IMain::USER_COMMAND_SAVE_RIG_FILE);
		enum_cmd_proxy.AddField("USER_COMMAND_CLOSE_CURRENT_RIG"					   , (int)IMain::USER_COMMAND_CLOSE_CURRENT_RIG);
		enum_cmd_proxy.AddField("USER_COMMAND_CREATE_NEW_EMPTY_RIG"					   , (int)IMain::USER_COMMAND_CREATE_NEW_EMPTY_RIG);
		enum_cmd_proxy.AddField("USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_NODES"	   , (int)IMain::USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_NODES);
		enum_cmd_proxy.AddField("USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_BEAMS"	   , (int)IMain::USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_BEAMS);
		enum_cmd_proxy.AddField("USER_COMMAND_QUIT_RIG_EDITOR"						   , (int)IMain::USER_COMMAND_QUIT_RIG_EDITOR);
		enum_cmd_proxy.AddField("USER_COMMAND_SHOW_RIG_PROPERTIES_WINDOW"			   , (int)IMain::USER_COMMAND_SHOW_RIG_PROPERTIES_WINDOW);
		enum_cmd_proxy.AddField("USER_COMMAND_SAVE_CONTENT_OF_RIG_PROPERTIES_WINDOW"   , (int)IMain::USER_COMMAND_SAVE_CONTENT_OF_RIG_PROPERTIES_WINDOW);
		enum_cmd_proxy.AddField("USER_COMMAND_SHOW_LAND_VEHICLE_PROPERTIES_WINDOW"	   , (int)IMain::USER_COMMAND_SHOW_LAND_VEHICLE_PROPERTIES_WINDOW);
		enum_cmd_proxy.AddField("USER_COMMAND_SAVE_LAND_VEHICLE_PROPERTIES_WINDOW_DATA", (int)IMain::USER_COMMAND_SAVE_LAND_VEHICLE_PROPERTIES_WINDOW_DATA);
		enum_cmd_proxy.AddField("USER_COMMAND_SHOW_HELP_WINDOW"						   , (int)IMain::USER_COMMAND_SHOW_HELP_WINDOW);
		enum_cmd_proxy.AddField("USER_COMMAND_INVALID"								   , (int)IMain::USER_COMMAND_INVALID);

		A.RegisterInterface       ("IRigEditorUserCommandCallbackListener_UGLY");
		A.RegisterInterfaceMethod ("IRigEditorUserCommandCallbackListener_UGLY", "void HandleUglyUserCommand(int command)");

		A.RegisterFuncdef        ("void FRigEditorUserCommandCallback_UGLY(int cmd)");
		class_main_proxy.AddMethod("void RegisterUserCommandCallback_UGLY(IRigEditorUserCommandCallbackListener_UGLY@ obj, string method_name)",
			asMETHOD(RigEditor::Main, AS_RegisterUserCommandCallback_UGLY));

		// GUI
		MyGUI_AngelScriptExport::Export(&A);
		GUI::OpenSaveFileDialog::BindToAngelScript(&A);

		// RoR system interface
		A.RegisterGlobalFunction("RigEditorCore_UGLY@ SYS_GetRigEditorInstance_UGLY()",    asFUNCTION(AS_GetRigEditorInstance),         asCALL_CDECL);
		A.RegisterGlobalFunction("bool                SYS_IsRoRApplicationWindowClosed()", asFUNCTION(AS_IsRoRApplicationWindowClosed), asCALL_CDECL);
		A.RegisterGlobalFunction("bool                SYS_RequestRoRShutdown()",           asFUNCTION(AS_RequestRoRShutdown),           asCALL_CDECL);
		A.RegisterGlobalFunction("bool                SYS_RenderFrameAndUpdateWindow()",   asFUNCTION(AS_RenderFrameAndUpdateWindow),   asCALL_CDECL);
		A.RegisterGlobalFunction("string              SYS_GetStringSetting(string key, string default_val)",
			asFUNCTION(AS_SYS_GetStringSetting), asCALL_CDECL);
		A.RegisterGlobalFunction("string              SYS_LoadRigEditorResourceAsString(string filename)",
			asFUNCTION(AS_SYS_LoadRigEditorResourceAsString), asCALL_CDECL);
			

		return 0;
	}
	catch(std::runtime_error ex)
	{
		m_log->logMessage(std::string("An exception occured during registering AngelScript interface, message: ") + ex.what());
		return -1;
	}
}

#endif

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

#if 0
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
#endif



#if 0
// Static helper
void ScriptEngine::PrepareAndExecuteFunction(asIScriptFunction* func, asIScriptContext* ctx, asIScriptEngine* engine)
{
    // Prepare the script context with the function we wish to execute. Prepare()
	// must be called on the context before each new script function that will be
	// executed.
	int result = ctx->Prepare(func);
	if( result < 0 ) 
	{
		std::stringstream msg;
        msg << __FUNCTION__ << "(): Failed to prepare AngelScript context, result: " << ContextPrepare_ErrorCodeToString(result);
        throw std::runtime_error(msg.str());
	}

	ScriptEngine::ExecuteContext(ctx, engine);	
}

 void        ScriptEngine::ExecuteContext(AngelScript::asIScriptContext* ctx, AngelScript::asIScriptEngine* engine)
{
	int result = ctx->Execute();

    
	if( result != asEXECUTION_FINISHED )
	{
		std::stringstream msg;
		// The execution didn't finish as we had planned. Determine why.
		if( result == asEXECUTION_ABORTED )
        {
            msg << "The execution was aborted with a call to Abort (asEXECUTION_ABORTED).";
        }
		else if( result == asEXECUTION_EXCEPTION )
		{
			msg << "The script ended with an exception." << endl;

			// Write some information about the script exception
            
			int funcId = ctx->GetExceptionFunction();
            const asIScriptFunction *func = engine->GetFunctionById(funcId);
			msg << "\tFunction: " << func->GetDeclaration()        << endl;
			msg << "\t  Module: " << func->GetModuleName()         << endl;
			msg << "\t Section: " << func->GetScriptSectionName()  << endl;
			msg << "\t LineNum: " << ctx->GetExceptionLineNumber() << endl;
			msg << "\t Message: " << ctx->GetExceptionString()     << endl;
		}
		else
        {
			msg << "The script ended for some unforeseen reason (result=" << result << ")." << endl;
        }
		throw std::runtime_error(msg.str());
	}
}


#endif

void LogPythonUnicodeObject(Ogre::Log* m_log, boost::python::object& py_unicode_obj)
{
	m_log->logMessage("LogPythonUnicodeObject(): START");

	if (py_unicode_obj.is_none())
	{
		m_log->logMessage("LogPythonUnicodeObject FAIL, py_unicode_obj is None");
		return;
	}
	m_log->logMessage("LogPythonUnicodeObject(): LINE DONE is_none()?");

	if (! PyUnicode_Check(py_unicode_obj.ptr()))
	{
		m_log->logMessage("LogPythonUnicodeObject():failed to get info :-( [ failed PyUnicode_Check test]");
		return;
	}
	m_log->logMessage("LogPythonUnicodeObject(): LINE DONE PyUnicode_Check ");

	if (! PyUnicode_READY(py_unicode_obj.ptr()))
	{
		m_log->logMessage("LogPythonUnicodeObject(): We are not PyUnicode_READY...");
		Py_UNICODE* w_str = PyUnicode_AS_UNICODE(py_unicode_obj.ptr());
		if (w_str == nullptr)
		{
			m_log->logMessage("LogPythonUnicodeObject(): PyUnicode_AS_UNICODE() gave NULL :-(");
			return;
		}
		MyGUI::UString u_str(w_str);

		m_log->logMessage("LogPythonUnicodeObject(): OUTPUT:");
		m_log->logMessage(u_str);
		return;
	}
	int kind = PyUnicode_KIND(py_unicode_obj.ptr());
	m_log->logMessage(std::string("LogPythonUnicodeObject(): PyUnicode_READY OK! Kind: ") 
		+ PythonUnicodeKindToString(kind));
	
	Py_UCS1* ucs1 = nullptr;
	Py_UCS2* ucs2 = nullptr;
	Py_UCS4* ucs4 = nullptr;

	m_log->logMessage("LogPythonUnicodeObject(): OUTPUT:");
	switch (kind)
	{
		case PyUnicode_WCHAR_KIND: break;

		case PyUnicode_1BYTE_KIND:
		{
			ucs1 = PyUnicode_1BYTE_DATA(py_unicode_obj.ptr()); m_log->logMessage((char*)ucs1); break;
		}

		case PyUnicode_2BYTE_KIND: 
		{
			ucs2 = PyUnicode_2BYTE_DATA(py_unicode_obj.ptr()); 
			MyGUI::UString u_str((wchar_t*)ucs2);
			m_log->logMessage(u_str); 
			break;
		}

		case PyUnicode_4BYTE_KIND:
		{ 
			ucs4 = PyUnicode_4BYTE_DATA(py_unicode_obj.ptr()); break;
		}
	}


}

void LogPythonStdOutput(Ogre::Log* m_log, boost::python::object& main_namespace)
{
	// https://docs.python.org/3/library/io.html#io.StringIO
	using namespace boost::python;

	m_log->logMessage("LogPythonStdOutput(): START");

	m_log->logMessage("LogPythonStdOutput(): getting stdout");
	object stdout_str = eval("sys.stdout.getvalue()", main_namespace);
	m_log->logMessage("LogPythonStdOutput(): logging stdout");
	LogPythonUnicodeObject(m_log, stdout_str);
	
	m_log->logMessage("LogPythonStdOutput(): getting stderr");
	object stderr_str = eval("sys.stderr.getvalue()", main_namespace);
	m_log->logMessage("LogPythonStdOutput(): logging stderr");
	LogPythonUnicodeObject(m_log, stderr_str);

	m_log->logMessage("LogPythonStdOutput(): END");
}

void PythonExample(Ogre::Log* m_log)
{
	using namespace boost::python;

	// Example from docs
	object main_module = import("__main__");
	m_log->logMessage("PythonExample: import(__main__) DONE");
	object main_namespace = main_module.attr("__dict__");
	m_log->logMessage("PythonExample: main_module.attr(__dict__) DONE");

	object res;
	res = exec("import io, sys"                           , main_namespace);    m_log->logMessage("LINE import");
	res = exec("sys.stdout = io.StringIO()"               , main_namespace);    m_log->logMessage("LINE set stdout");
	res = exec("sys.stderr = io.StringIO()"               , main_namespace);    m_log->logMessage("LINE set stderr");
	res = exec("print(\"python stdio test!\")"            , main_namespace);    m_log->logMessage("LINE test stdio");
	res = exec("sys.stderr.write(\"python sterr test!\")" , main_namespace);    m_log->logMessage("LINE test stderr");

	LogPythonStdOutput(m_log, main_namespace);

	/*object ignored = exec("hello = file('hello_rig_editor.txt', 'w')\n"
						"hello.write('Hello rig editor!')\n"
						"hello.close()",
						main_namespace);*/

	m_log->logMessage("PythonExample: exec() DONE");
}


// From: http://www.gamedev.net/topic/491813
void ScriptEngine::LogPythonException()
{
	using namespace boost::python;
	PyErr_Print();
	object sys(handle<>(PyImport_ImportModule("sys")));
	object err = sys.attr("stderr");
	if (err.is_none())
	{
		m_log->logMessage("failed to get info :-( [sys.stderr == None]");
		return;
	}

	m_log->logMessage("Python exception info:");
	object stderr_val = err.attr("getvalue")();
	if (stderr_val.is_none())
	{
		m_log->logMessage("failed to get info :-( [stderr_val == None]");
		return;
	}
	if (! PyUnicode_Check(stderr_val.ptr()))
	{
		m_log->logMessage("failed to get info :-( [stderr_val failed PyUnicode_Check test]");
		return;
	}
	if (! PyUnicode_READY(stderr_val.ptr()))
	{
		m_log->logMessage("failed to get info :-( [stderr_val failed PyUnicode_READY test]");
		return;
	}
	m_log->logMessage("sys.stderr object KIND: ");
	m_log->logMessage(PythonUnicodeKindToString(PyUnicode_KIND(stderr_val.ptr())));
	auto err_text = extract<std::wstring>(stderr_val);
	m_log->logMessage(MyGUI::UString(err_text));
}

/*{
// From: http://stackoverflow.com/a/30155747 - fails at line attr(format_exception)
	using namespace boost::python;
  // acquire the Global Interpreter Lock

  PyObject * extype, * value, * traceback;
  PyErr_Fetch(&extype, &value, &traceback);
  if (!extype)
  {
	m_log->logMessage("Could not obtain exception info.");
  }

  object o_extype(handle<>(borrowed(extype)));
  object o_value(handle<>(borrowed(value)));
  object o_traceback(handle<>(borrowed(traceback)));

  object mod_traceback = import("traceback");
  object lines = mod_traceback.attr("format_exception")(
    o_extype, o_value, o_traceback);

	std::stringstream os;
	os << "Python exception info:\n";
  for (int i = 0; i < len(lines); ++i)
    os << extract<std::string>(lines[i])();

  m_log->logMessage(os.str());
}*/

bool ScriptEngine::EnterRigEditor()
{
    try
	{
		// Execute the Main() function
		m_log->logMessage("Executing the rig editor script");
		m_log->logMessage("==================================================");
		PythonExample(m_log);
		m_log->logMessage("==================================================");
		m_log->logMessage("The rig editor script has finished");
		return true;
	}
	catch (boost::python::error_already_set e)
	{
		

		m_log->logMessage("==================================================");
		m_log->logMessage("An <boost::python::error_already_set> exception occured, getting details...");

		this->LogPythonException();
		return false;
	}
	catch (std::runtime_error e)
	{
		m_log->logMessage("==================================================");
		m_log->logMessage("An <std::runtime_error> exception occured, message:");
		m_log->logMessage(e.what());
		return false;
	}
}

void ScriptEngine::ShutDown()
{

/*    if (m_context != nullptr && m_engine != nullptr)
    {
        m_main_function = nullptr;

	    m_context->Release();
        m_context = nullptr;

        // TODO: UPDATE DEPS AND USE: ShutDownAndRelease()
	    m_engine->Release();
        m_engine = nullptr;
    }
	*/

	if (m_log != nullptr)
	{
		m_log->logMessage("ScriptEngine: Shutting down.");
		delete m_log;
		m_log = nullptr;
	}
	
}

void ScriptEngine::LogMessage(std::string & msg)
{
    m_log->logMessage(msg);
}

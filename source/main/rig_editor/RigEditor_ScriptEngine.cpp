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
	@date   08/2015
	@author Petr Ohlidal
*/

#include "RigEditor_ScriptEngine.h"

#include "Application.h"
#include "ContentManager.h"
#include "Settings.h"
#include "MainThread.h"
#include "OgreSubsystem.h"
#include "PlatformUtils.h"
#include "PythonHelper.h"
#include "RigEditor_Config.h"
#include "RigEditor_LineListDynamicMesh.h"
#include "RigEditor_Main.h"
#include "RigEditor_PointListDynamicMesh.h"
#include "RoRPrerequisites.h"
#include "RoRWindowEventUtilities.h"

#include <OgreManualObject.h>
#include <OgreRoot.h>
#include <OgreRenderWindow.h>

#include <boost/python/detail/wrap_python.hpp> // Replaces #include <Python.h>, recommended in Boost docs
#include <boost/python.hpp>

namespace RoR { namespace RigEditor {
	struct ScriptEngineImpl
	{
		boost::python::object            py_main_module;
		boost::python::object            py_main_namespace;
	};
} } // namespace RoR { namespace RigEditor {

using namespace RoR;
using namespace RigEditor;
using namespace std;
using namespace boost::python;

// -----------------------------------------------------------------------------
// Module "ror_system"

void PY_EnterRigEditor()
{
	auto rig_editor = GetRigEditorGlobalInstance();
	rig_editor->PY_OnEnter_SetupInput();
	rig_editor->PY_OnEnter_SetupCameraAndViewport();
}

void PY_RenderFrameAndUpdateWindow()
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	RoRWindowEventUtilities::messagePump();
#endif

	RoR::Application::GetOgreSubsystem()->GetOgreRoot()->renderOneFrame();
	Ogre::RenderWindow* rw = RoR::Application::GetOgreSubsystem()->GetRenderWindow();
	if (!rw->isActive() && rw->isVisible())
	{
		rw->update(); // update even when in background !
	}
}

BOOST_PYTHON_MODULE(ror_system)
{
	using namespace boost::python;
	def("enter_rig_editor",               PY_EnterRigEditor);
	def("render_frame_and_update_window", PY_RenderFrameAndUpdateWindow);
}

// -----------------------------------------------------------------------------
// Module "ror_drawing"

PointListDynamicMesh* PY_CreateDynamicMeshOfPoints(float point_size)
{
	return new PointListDynamicMesh(GetRigEditorGlobalInstance(), point_size, 10);
}

LineListDynamicMesh* PY_CreateDynamicMeshOfLines()
{
	return new LineListDynamicMesh(GetRigEditorGlobalInstance(), 10);
}

BOOST_PYTHON_MODULE(ror_drawing)
{
	using namespace boost::python;

	class_<PointListDynamicMesh>("PointMesh", no_init)
		.def("set_position",      &PointListDynamicMesh::PY_SetPosition)
		.def("attach_to_scene",   &PointListDynamicMesh::AttachToScene)
		.def("detach_from_scene", &PointListDynamicMesh::DetachFromScene)
		.def("begin_update",      &PointListDynamicMesh::BeginUpdate)
		.def("begin_update",      &PointListDynamicMesh::EndUpdate)
		.def("add_point",         &PointListDynamicMesh::PY_AddPoint);

	class_<LineListDynamicMesh>("LineMesh", no_init)
		.def("set_position",      &LineListDynamicMesh::PY_SetPosition)
		.def("attach_to_scene",   &LineListDynamicMesh::AttachToScene)
		.def("detach_from_scene", &LineListDynamicMesh::DetachFromScene)
		.def("begin_update",      &LineListDynamicMesh::BeginUpdate)
		.def("begin_update",      &LineListDynamicMesh::EndUpdate)
		.def("add_line",          &LineListDynamicMesh::PY_AddLine);

	def("create_point_mesh", PY_CreateDynamicMeshOfPoints, return_value_policy<reference_existing_object>());
	def("create_line_mesh",  PY_CreateDynamicMeshOfLines,  return_value_policy<reference_existing_object>());
}

// -----------------------------------------------------------------------------
// class ScriptEngine

ScriptEngine::ScriptEngine() :
	m_rig_editor_instance(nullptr),
	m_log(nullptr),
	m_impl(nullptr)
{
}

void ScriptEngine::Bootstrap()
{
	m_log = Ogre::LogManager::getSingleton().createLog(SSETTING("Log Path", "") + "/RigEditorScriptEngine.log", false);

	// Import the module. Source: https://wiki.python.org/moin/boost.python/EmbeddingPython
	PyImport_AppendInittab("ror_system",  PyInit_ror_system);  // Function "PyInit_ror_system" defined by BOOST_PYTHON_MODULE
	PyImport_AppendInittab("ror_drawing", PyInit_ror_drawing); // Function "PyInit_ror_drawing" defined by BOOST_PYTHON_MODULE
	// start the interpreter and create the __main__ module. Source: http://www.boost.org/doc/libs/1_59_0/libs/python/doc/tutorial
	Py_Initialize();
	// Create main module. Source: http://www.boost.org/doc/libs/1_59_0/libs/python/doc/tutorial
	object main_module = import("__main__");
	object main_namespace = main_module.attr("__dict__");

	/*m_impl = new ScriptEngineImpl();
	m_impl->py_main_module = main_module;
	m_impl->py_main_namespace = main_namespace;

	main_namespace = m_impl->py_main_namespace;*/
	
	try
	{
		// Error reporting = redirect stdout/stderr to log files.
		// Buffering must be set to 1 (flush after every line) because we can't use Py_Finalize() 
		//     to close files - it's broken in boost 1_59,
		//     see http://www.boost.org/doc/libs/1_48_0/libs/python/doc/tutorial/doc/html/python/embedding.html
		// Path must use forward slashes '/' because '\' are Python escapes.
		std::string stdout_log_path = SSETTING("Log Path", "");
		std::string stderr_log_path = SSETTING("Log Path", "");
		PythonHelper::PathConvertSlashesToForward(stderr_log_path);
		PythonHelper::PathConvertSlashesToForward(stdout_log_path);
		stderr_log_path += "/RigEditorPythonStderr.log";
		stdout_log_path += "/RigEditorPythonStdout.log";
		std::string stdout_statement = std::string("sys.stdout = open('") + stdout_log_path + "', 'w', buffering=1)";
		std::string stderr_statement = std::string("sys.stderr = open('") + stderr_log_path + "', 'w', buffering=1)";

		m_log->logMessage("DEBUG: log file paths:");
		m_log->logMessage(stdout_log_path);
		m_log->logMessage(stderr_log_path);

		object res;
		res = exec("import io, sys", main_namespace);
		res = exec(stdout_statement.c_str(), main_namespace);
		res = exec(stderr_statement.c_str(), main_namespace);
		res = exec("print('Rig Editor: Python standard output (stdout)\\n')", main_namespace);
		res = exec("sys.stderr.write('Rig Editor: Python standard error output (stderr)\\n')", main_namespace);

		//res = exec("5/0", main_namespace);// Test = force exception


		// TMP - test
		CreateRigEditorGlobalInstance(); // Global function

		std::string main_script_path = SSETTING("RigEditor Scripts Path", "");
		PythonHelper::PathConvertSlashesToForward(main_script_path);
		main_script_path += "/Main.py";
		object result = boost::python::exec_file(main_script_path.c_str(), main_namespace); // TMP - TEST
	}
	catch (error_already_set)
	{
		m_log->logMessage("Bootstrap(): FATAL ERROR: Failed to redirect standard output to files.");
		return;
	}

	m_log->logMessage("DEBUG Bootstrap() is done, ready to launch RigEditor");

	

}

bool ScriptEngine::EnterRigEditor()
{
	if (m_impl == nullptr)
	{
		return false;
	}
	try
	{
		

		// Execute the Main() function
		m_log->logMessage("Executing the rig editor script");
		//m_log->logMessage(main_script_path);
		m_log->logMessage("==================================================");
		
		m_log->logMessage("==================================================");
		m_log->logMessage("The rig editor script has finished");
		return true;
	}
	catch (boost::python::error_already_set e)
	{
		m_log->logMessage("==================================================");
		m_log->logMessage("An <boost::python::error_already_set> exception occured.");
		m_log->logMessage("Inspect 'logs/RigEditorPythonStderr.log' for details");
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
	if (m_log != nullptr)
	{
		m_log->logMessage("ScriptEngine: Shutting down.");
		delete m_log;
		m_log = nullptr;
	}
	if (m_impl != nullptr)
	{
		delete m_impl;
	}
}

// ###################################################################################################
// ###################################################################################################
// Dirty AngelScript-related code, to be cleaned
// ###################################################################################################
// ###################################################################################################

#if 0

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
#endif






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





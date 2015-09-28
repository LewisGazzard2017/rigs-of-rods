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
#include "InputEngine.h"
#include "MainThread.h"
#include "OgreSubsystem.h"
#include "PlatformUtils.h"
#include "PythonHelper.h"
#include "RigEditor_Config.h"
#include "RigEditor_LineListDynamicMesh.h"
#include "RigEditor_Main.h"
#include "RigEditor_PointListDynamicMesh.h"
#include "RigEditor_CameraWrapper.h"
#include "RoRPrerequisites.h"
#include "RoRWindowEventUtilities.h"
#include "Settings.h"

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

bool PY_IsRoRApplicationWindowClosed()
{
	return RoR::Application::GetOgreSubsystem()->GetRenderWindow()->isClosed();
}

void PY_RequestRoRShutdown()
{
	RoR::Application::GetMainThreadLogic()->RequestShutdown();
}

void PY_CaptureInputAndUpdateGUI()
{
	RoR::Application::GetInputEngine()->Capture(); // Also injects input to GUI (through RigEditor::InputHandler)
}

void PY_RegisterInputListener(object listener)
{
	GetRigEditorGlobalInstance()->GetInputHandler().SetPythonInputListener(listener);
}

RigEditor::CameraWrapper* PY_GetCamera()
{
	return GetRigEditorGlobalInstance()->GetCameraWrapper();
}

BOOST_PYTHON_MODULE(ror_system)
{
	using namespace boost::python;

	class_<CameraWrapper>("Camera", no_init)
		.def("look_at",                          &CameraWrapper::PY_LookAt)
		.def("yaw_degrees",                      &CameraWrapper::PY_YawDegrees)
		.def("roll_degrees",                     &CameraWrapper::PY_RollDegrees)
		.def("set_position",                     &CameraWrapper::PY_SetPosition)
		.def("pitch_degrees",                    &CameraWrapper::PY_PitchDegrees)
		.def("is_mode_ortho",                    &CameraWrapper::PY_IsModeOrtho)
		.def("move_relative",                    &CameraWrapper::PY_MoveRelative)
		.def("set_mode_ortho",                   &CameraWrapper::PY_SetModeOrtho)
		.def("toggle_mode_ortho",                &CameraWrapper::PY_ToggleModeOrtho)
		.def("set_fov_y_degrees",                &CameraWrapper::PY_SetFOVyDegrees)
		.def("get_point_z_distance",             &CameraWrapper::PY_PointZDistance)
		.def("set_far_clip_distance",            &CameraWrapper::PY_SetFarClipDistance)
		.def("set_near_clip_distance",           &CameraWrapper::PY_SetNearClipDistance)
		.def("set_ortho_window_width",           &CameraWrapper::PY_SetOrthoWindowWidth)
		.def("convert_world_to_screen_position", &CameraWrapper::PY_ConvertWorldToScreenPosition)
		.def("convert_screen_to_world_position", &CameraWrapper::PY_ConvertScreenToWorldPosition);

	def("get_camera", &PY_GetCamera, return_value_policy<reference_existing_object>());

	def("enter_rig_editor",               PY_EnterRigEditor);
	def("render_frame_and_update_window", PY_RenderFrameAndUpdateWindow);
	def("is_application_window_closed",   PY_IsRoRApplicationWindowClosed);
	def("request_application_shutdown",   PY_RequestRoRShutdown);
	def("capture_input_and_update_gui",   PY_CaptureInputAndUpdateGUI);
	def("register_input_listener",        PY_RegisterInputListener);
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

	class_<PointListDynamicMesh>("PointsMesh", no_init)
		.def("set_position",      &PointListDynamicMesh::PY_SetPosition)
		.def("attach_to_scene",   &PointListDynamicMesh::PY_AttachToScene)
		.def("detach_from_scene", &PointListDynamicMesh::DetachFromScene)
		.def("begin_update",      &PointListDynamicMesh::BeginUpdate)
		.def("end_update",        &PointListDynamicMesh::EndUpdate)
		.def("add_point",         &PointListDynamicMesh::PY_AddPoint);

	class_<LineListDynamicMesh>("LinesMesh", no_init)
		.def("set_position",      &LineListDynamicMesh::PY_SetPosition)
		.def("attach_to_scene",   &LineListDynamicMesh::PY_AttachToScene)
		.def("detach_from_scene", &LineListDynamicMesh::DetachFromScene)
		.def("begin_update",      &LineListDynamicMesh::BeginUpdate)
		.def("end_update",        &LineListDynamicMesh::EndUpdate)
		.def("add_line",          &LineListDynamicMesh::PY_AddLine);

	def("create_points_mesh", PY_CreateDynamicMeshOfPoints, return_value_policy<reference_existing_object>());
	def("create_lines_mesh",  PY_CreateDynamicMeshOfLines,  return_value_policy<reference_existing_object>());
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
	m_impl = new ScriptEngineImpl();
	m_impl->py_main_module = import("__main__");
	m_impl->py_main_namespace = m_impl->py_main_module.attr("__dict__");
	
	try
	{
		// Error reporting = redirect stdout/stderr to log files.
		// Buffering must be set to 1 (flush after every line) because we can't use Py_Finalize() 
		//     to close files - it's broken in boost 1_59,
		//     see http://www.boost.org/doc/libs/1_48_0/libs/python/doc/tutorial/doc/html/python/embedding.html
		auto log_dir = this->GetConfigPath("Log Path");
		char py_code[2000];
		sprintf(py_code,
			"import io, sys                                                            \n"
			"sys.stdout = open('%s/RigEditorPythonStdout.log', 'w', buffering=1)       \n"
			"sys.stderr = open('%s/RigEditorPythonStderr.log', 'w', buffering=1)       \n"
			"print('Rig Editor: Python standard output (stdout)\\n')                   \n"
			"sys.stderr.write('Rig Editor: Python standard error output (stderr)\\n')  \n"
			"sys.path.append('%s')                                                     \n", // Make local modules visible to "import"
			log_dir.c_str(), 
			log_dir.c_str(), 
			this->GetConfigPath("RigEditor Scripts Path").c_str()
			);
		object res_ignored = exec(py_code, m_impl->py_main_namespace);
	}
	catch (error_already_set)
	{
		m_log->logMessage("Bootstrap(): FATAL ERROR: Failed to redirect standard output to files.");
		delete m_impl;
		return;
	}

	CreateRigEditorGlobalInstance(); // Global function
	m_log->logMessage("RigEditor/ScriptEngine: Startup is finished.");
}

bool ScriptEngine::EnterRigEditor()
{
	if (m_impl == nullptr)
	{
		return false;
	}
	try
	{
		std::string path = this->GetConfigPath("RigEditor Scripts Path") + "/main.py";
		m_log->logMessage("Executing the rig editor script");
		m_log->logMessage("==================================================");
		object res_ignored = boost::python::exec_file(path.c_str(), m_impl->py_main_namespace);
		m_log->logMessage("==================================================");
		m_log->logMessage("The rig editor script has finished");
		return true;
	}
	catch (boost::python::error_already_set e)
	{
		m_log->logMessage("==================================================");
		m_log->logMessage("An <boost::python::error_already_set> exception occured.");
		m_log->logMessage("Inspect 'logs/RigEditorPythonStderr.log' for details");
		PyErr_Print(); // Prints error to python's standard error output (stderr)
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
		m_log->logMessage("RigEditor/ScriptEngine: Shutting down.");
		delete m_log;
		m_log = nullptr;
	}
	if (m_impl != nullptr)
	{
		delete m_impl;
		m_impl = nullptr;
	}
}

std::string ScriptEngine::GetConfigPath(const char* config_key)
{
	// Path must use forward slashes '/' because '\' are Python escapes.
	std::string path = SSETTING(config_key, "");
	PythonHelper::PathConvertSlashesToForward(path);
	return path;
}

void ScriptEngine::Vector3_ToPython(Ogre::Vector3& v, void* boost_python_object_ptr)
{
	object res = eval("Vector3()", this->m_impl->py_main_namespace);
	res['x'] = v.x;
	res['y'] = v.y;
	res['z'] = v.z;

	object* target = static_cast<object*>(boost_python_object_ptr);
	*target = res;
}

// ###################################################################################################
// ###################################################################################################
// Dirty AngelScript-related code, to be cleaned
// ###################################################################################################
// ###################################################################################################

#if 0

std::string AS_GetStringSetting(std::string key, std::string default_val)
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


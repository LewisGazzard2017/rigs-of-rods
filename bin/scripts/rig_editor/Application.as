// Rig editor application class

class Application:
    IRigEditorUserCommandCallbackListener_UGLY
{
    // Interface: IRigEditorUserCommandCallbackListener_UGLY
    // Funcdef: void FRigEditorUserCommandCallback_UGLY(int cmd)    
    void HandleUglyUserCommand(int command)
    {
        LogMessage("HandleUglyUserCommand() invoked");
        switch (command)
        {   
            case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_DIALOG_OPEN_RIG_FILE:
                m_rig_editor_core.HandleCommandShowDialogOpenRigFile_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_DIALOG_SAVE_RIG_FILE_AS:
                m_rig_editor_core.HandleCommandShowDialogSaveRigFileAs_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SAVE_RIG_FILE:
                // To be implemented...
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_CLOSE_CURRENT_RIG:
                m_rig_editor_core.HandleCommandCloseCurrentRig_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_CREATE_NEW_EMPTY_RIG:
                m_rig_editor_core.HandleCommandCreateNewEmptyRig_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_NODES:
                m_rig_editor_core.HandleCommandCurrentRigDeleteSelectedNodes_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_BEAMS:
                m_rig_editor_core.HandleCommandCurrentRigDeleteSelectedBeams_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_QUIT_RIG_EDITOR:
                m_rig_editor_core.HandleCommandQuitRigEditor_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_RIG_PROPERTIES_WINDOW:
                m_rig_editor_core.HandleCommandShowRigPropertiesWindow_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SAVE_CONTENT_OF_RIG_PROPERTIES_WINDOW:
                m_rig_editor_core.HandleCommandSaveContentOfRigPropertiesWindow_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_LAND_VEHICLE_PROPERTIES_WINDOW:
                m_rig_editor_core.HandleCommandShowLandVehiclePropertiesWindow_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SAVE_LAND_VEHICLE_PROPERTIES_WINDOW_DATA:
                m_rig_editor_core.HandleCommandSaveLandVehiclePropertiesWindowData_UGLY();
                break;
                
            case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_HELP_WINDOW:
                m_rig_editor_core.HandleCommandShowHelpWindow_UGLY();
                break;
        }
        LogMessage("HandleUglyUserCommand() finishes");    
    }

    // Constructor
    Application()
    {
        @m_rig_editor_core = SYS_GetRigEditorInstance_UGLY();
        if (m_rig_editor_core !is null)
        {
            LogMessage("Application(): RigEditorCore_UGLY instance acquired");
        }
        else
        {
            LogMessage("Application(): ERROR, GetRigEditorInstance_UGLY() returned NULL handle!!!");
        }
        
        m_rig_editor_core.RegisterUserCommandCallback_UGLY(@this, "HandleUglyUserCommand");
        LogMessage("Application() - User command callback registered.");
    }
    
    void Go()
    {
        LogMessage("Application::Go() - Enter");
        // Perform setup
        // TIP: Don't change order, it's all temporary and unsafe
        m_rig_editor_core.OnEnter_SetupCameraAndViewport_UGLY();
        LogMessage("Application::Go() - OnEnter_SetupCameraAndViewport_UGLY() finished");
        m_rig_editor_core.OnEnter_InitializeOrRestoreGui_UGLY();
        LogMessage("Application::Go() - OnEnter_InitializeOrRestoreGui_UGLY() finished");
        m_rig_editor_core.OnEnter_SetupInput_UGLY();
        LogMessage("Application::Go() - OnEnter_SetupInput_UGLY() finished");
        
        // Run the main loop
        m_exit_requested = false;
        while (!m_exit_requested and !m_rig_editor_core.WasExitLoopRequested_UGLY())
        {
            if (SYS_IsRoRApplicationWindowClosed())
            {
                SYS_RequestRoRShutdown();
                m_exit_requested = true;
                continue;
            }
            
            m_rig_editor_core.UpdateMainLoop_UGLY();
            
            SYS_RenderFrameAndUpdateWindow();
        }
        
        m_rig_editor_core.OnExit_HideGui_UGLY();
        m_rig_editor_core.OnExit_ClearExitRequest_UGLY();
    }
    
    

    
    // ===== Variables =====
    
    private RigEditorCore_UGLY@ m_rig_editor_core;
    private bool                m_exit_requested;
    // More to come...
}

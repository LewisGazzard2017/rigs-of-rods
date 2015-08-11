// Rig editor application class

#include "UserCommandHandler.as"

class Application
{
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
        
        @m_user_command_handler = UserCommandHandler(@m_rig_editor_core);
        m_rig_editor_core.RegisterUserCommandCallback_UGLY(
            @m_user_command_handler, HandleUglyUserCommand);
        LogMessage("Application::Go() - User command callback registered.");
        
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
    private UserCommandHandler@ m_user_command_handler;
    // More to come...
}

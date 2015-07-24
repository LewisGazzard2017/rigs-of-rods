// Rig editor application class

class Application
{
    Application()
    {
        @m_rig_editor_core = GetRigEditorInstance_UGLY();
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
        
        // Run the main loop
        // TODO: Implement the main loop in script!!!
        m_rig_editor_core.OnEnter_RunMainLoop_UGLY();
    }
    
    // ===== Variables =====
    
    private RigEditorCore_UGLY@ m_rig_editor_core;
    // More to come...
}
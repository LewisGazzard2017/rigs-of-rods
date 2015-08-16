
#include "GuiPanels.as"

class GuiManager: GUI_IOpenSaveFileDialogListener
{
    GuiManager(Application@ app)
    {
        @m_application = @app;
        @m_open_save_file_dialog = GUI_OpenSaveFileDialog();
        LogMessage("GuiManager created OK");
    }
    
    // -------------------------------------------------------------------------
    // Overrides GUI_IOpenSaveFileDialogListener
    // Funcdef: void FOpenSaveFileDialogFinishedCallback(bool success, string folder, string filename)
    private void OpenRigFileDialogFinished(bool success, string folder, string filename)
    {
        if (success)
        {
            m_application.LoadRigDefFile(folder, filename);
        }
        m_open_save_file_dialog.EndModal(); // Hide dialog
    }
    
    // -------------------------------------------------------------------------
    // Overrides GUI_IOpenSaveFileDialogListener
    // Funcdef: void FOpenSaveFileDialogFinishedCallback(bool success, string folder, string filename)
    private void SaveRigFileDialogFinished(bool success, string folder, string filename)
    {
        if (success)
        {
            m_application.SaveRigDefFile(folder, filename);
        }
        m_open_save_file_dialog.EndModal(); // Hide dialog
    }
    
    // -------------------------------------------------------------------------
    void ShowOpenRigFileDialog()
    {
        bool directory_mode = false;
        m_open_save_file_dialog.ConfigureDialog("Open rig file", "Open", directory_mode);
        m_open_save_file_dialog.RegisterDialogFinishedCallback(@this, "OpenRigFileDialogFinished");
        m_open_save_file_dialog.StartModal(); // Shows the dialog
    }
    
    // -------------------------------------------------------------------------
    void ShowSaveRigFileDialog()
    {
        bool directory_mode = false;
        m_open_save_file_dialog.ConfigureDialog("Save rig file", "Save", directory_mode);
        m_open_save_file_dialog.RegisterDialogFinishedCallback(@this, "SaveRigFileDialogFinished");
        m_open_save_file_dialog.StartModal(); // Shows the dialog
    }
    
    // -------------------------------------------------------------------------
    void ShowWindowIfHiddenTemporarily(GUI_WindowBase@ window)
    {
        if (window !is null)
        {
            window.ShowIfHiddenTemporarily();
        }
    }
    
    // -------------------------------------------------------------------------
    void InitOrRestoreGui()
    {
        if (m_help_window is null)
        {
            @m_help_window = GUI_HelpWindow();
            // TODO: Internationalize
            m_help_window.SetText(SYS_LoadRigEditorResourceAsString("rig_editor_helpfile_english.txt"));
        }
        ShowWindowIfHiddenTemporarily(m_help_window);
    }
    
    // -------------------------------------------------------------------------
    void HideGui()
    {
        m_help_window.HideTemporarily();
    }
    
    // -------------------------------------------------------------------------
    private GUI_OpenSaveFileDialog@   m_open_save_file_dialog;
    private GUI_HelpWindow@           m_help_window;
    private Application@              m_application;
}
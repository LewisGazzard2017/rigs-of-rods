// Class receiving "user commands" from RigEditorCore_UGLY and handling them.
// Must inherit from interface IRigEditorUserCommandCallbackListener_UGLY

class UserCommandHandler: IRigEditorUserCommandCallbackListener_UGLY
{
    // Constructor
    UserCommandHandler(RigEditorCore_UGLY@ core)
    {
        @m_rig_editor_core = @core;
    }
    
    RigEditorCore_UGLY@ GetCore_UGLY()
    {
        return @m_rig_editor_core;
    }

    // Variables
    private RigEditorCore_UGLY@ m_rig_editor_core;
}

// Command handler callback
// Must comply to funcdef: 
//     FRigEditorUserCommandCallback_UGLY(IRigEditorUserCommandCallbackListener_UGLY@ obj, int cmd)
void HandleUglyUserCommand(IRigEditorUserCommandCallbackListener_UGLY@ handler, int command)
{
    // TODO: Use enum RigEditorUserCommand_UGLY
    LogMessage("HandleUglyUserCommand() invoked");
    RigEditorCore_UGLY@ core = handler.GetCore_UGLY();
    switch (command)
    {   
        case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_DIALOG_OPEN_RIG_FILE:
            core.HandleCommandShowDialogOpenRigFile_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_DIALOG_SAVE_RIG_FILE_AS:
            core.HandleCommandShowDialogSaveRigFileAs_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SAVE_RIG_FILE:
            // To be implemented...
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_CLOSE_CURRENT_RIG:
            core.HandleCommandCloseCurrentRig_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_CREATE_NEW_EMPTY_RIG:
            core.HandleCommandCreateNewEmptyRig_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_NODES:
            core.HandleCommandCurrentRigDeleteSelectedNodes_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_CURRENT_RIG_DELETE_SELECTED_BEAMS:
            core.HandleCommandCurrentRigDeleteSelectedBeams_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_QUIT_RIG_EDITOR:
            core.HandleCommandQuitRigEditor_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_RIG_PROPERTIES_WINDOW:
            core.HandleCommandShowRigPropertiesWindow_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SAVE_CONTENT_OF_RIG_PROPERTIES_WINDOW:
            core.HandleCommandSaveContentOfRigPropertiesWindow_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_LAND_VEHICLE_PROPERTIES_WINDOW:
            core.HandleCommandShowLandVehiclePropertiesWindow_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SAVE_LAND_VEHICLE_PROPERTIES_WINDOW_DATA:
            core.HandleCommandSaveLandVehiclePropertiesWindowData_UGLY();
            break;
            
        case RigEditorUserCommand_UGLY::USER_COMMAND_SHOW_HELP_WINDOW:
            core.HandleCommandShowHelpWindow_UGLY();
            break;
    }
    LogMessage("HandleUglyUserCommand() finishes");    
}

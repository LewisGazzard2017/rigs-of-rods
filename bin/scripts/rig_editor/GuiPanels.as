
class GUI_HelpWindow
{
    GUI_HelpWindow()
    {
        string path = SYS_GetStringSetting("Resources Path") + "/rig_editor/rig_editor_help_window.layout";
        MyGUI_VectorWidgetPtr root_widgets = MyGUI_LayoutManager_LoadLayout(path, "");
        if (root_widgets.Size() == 0)
        {
            LogMessage("constructor GUI_HelpWindow(): failed to load layout: " + path);
            return;
        }
        @m_panel_widget = root_widgets.At(0);
    }
    
    private MyGUI_Widget@ m_panel_widget;
}
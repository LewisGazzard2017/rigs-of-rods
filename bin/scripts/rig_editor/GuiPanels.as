
class GUI_WindowBase
{
    void InitBaseWindow(MyGUI_Window@ root_widget)
    {
        @m_window = @root_widget;
        m_is_hidden_temporarily = false; 
    }
    
    void CenterToScreen()
    {
        MyGUI_IntSize parent_size = m_window.GetParentSize();
        MyGUI_IntSize window_size = m_window.GetSize();
        int x = (parent_size.width  - window_size.width)  / 2;
        int y = (parent_size.height - window_size.height) / 2;
        m_window.SetPosition(x, y);
    }
    
    void Show()
    {
        m_is_hidden_temporarily = false;
        m_window.SetVisible(true);
    }
    
    void HideTemporarily()
    {
        m_is_hidden_temporarily = true;
        m_window.SetVisible(false);
    }
    
    void ShowIfHiddenTemporarily()
    {
        if (m_is_hidden_temporarily)
        {
            this.Show();
        }
    }

    private MyGUI_Window@ m_window;
    private bool          m_is_hidden_temporarily;
}

class GUI_HelpWindow: GUI_WindowBase
{
    GUI_HelpWindow()
    {
        MyGUI_VectorWidgetPtr@ root_widgets 
            = MyGUI_LayoutManager_LoadLayout("rig_editor_help_window.layout", "", null);
        if (root_widgets.Size() == 0)
        {
            LogMessage("constructor GUI_HelpWindow(): failed to load layout: " + "rig_editor_help_window.layout");
            return;
        }
        this.InitBaseWindow(root_widgets.At(0).CastType_Window());
        @m_help_editbox = m_window.FindWidget("help_view").CastType_EditBox();
        this.CenterToScreen();
    }
    
    void SetText(string text)
    {
        m_help_editbox.SetCaption(text);        
        m_help_editbox.SetVScrollPosition(0); // Must be reset after filling-in text
    }
    
    private MyGUI_EditBox@ m_help_editbox;
}
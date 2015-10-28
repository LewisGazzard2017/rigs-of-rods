
import ror_gui

class GuiManager:

    def __init__(self):
        import gui_windows
        self.windows = {
            "menubar": gui_windows.Menubar
        }
    
    def init_or_restore_gui(self):
        import inspect
        for key in self.windows:
            val = self.windows[key]
            if inspect.isclass(val):
                self.windows[key] = val() # Instantiate the window class
            else:
                self.show_if_hidden_temporarily()
                
    def temporarily_hide_gui(self):
        for window in self.windows.values():
            window.hide_temporarily()
                 
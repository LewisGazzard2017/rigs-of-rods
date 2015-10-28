


class CrashReporter():
    
    def __init__(self):
        import ror_gui
        root_list = ror_gui.load_layout("rig_editor_crash_report_window.layout", "", None)
        self.root_widget = root_list[0]
        self.error_info_box = self.root_widget.find_widget("error_info_editbox").cast_type_EditBox()
        self.root_widget.set_visible(False)
        
    def show_error(self, text):
        self.error_info_box.set_caption(text)
        self.root_widget.set_visible(True)
        
    def center_to_screen(self):
        parent_size = self.root_widget.get_parent_size()
        this_size = self.root_widget.get_size()
        target_x = (parent_size.width  - this_size.width)  / 2
        target_y = (parent_size.height - this_size.height) / 2
        self.root_widget.set_position(int(target_x), int(target_y))        
        
    def report_current_exception(self):
        
        # Standard result of PyErr_PrintEx:
        # --------------------------------------
        #Traceback (most recent call last):
        #  File "D:/Projects/Git/RIGS-O~1/bin/scripts/rig_editor//main.py", line 14, in <module>
        #    app = application.Application()
        #  File "D:/Projects/Git/RIGS-O~1/bin/scripts/rig_editor\application.py", line 47, in __init__
        #    self.gui_manager = gui_manager.GuiManager() 
        #  File "D:/Projects/Git/RIGS-O~1/bin/scripts/rig_editor\gui_manager.py", line 7, in __init__
        #    import gui_windows
        #  File "D:/Projects/Git/RIGS-O~1/bin/scripts/rig_editor\gui_windows.py", line 50
        #    out_widgets = { "_Root_" = root_widget }
        #                             ^
        #SyntaxError: invalid syntax
        # --------------------------------------
        
        import sys, traceback
        
        text = traceback.format_exc()
        separator = "========================================"
        separator += separator
        separator =  "\n" + separator + "\n"
        sys.stderr.write(separator + text + separator)
        
        self.center_to_screen()
        self.show_error(text)   
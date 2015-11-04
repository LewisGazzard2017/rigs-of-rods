
import ror_gui



def load_gui_layout(filename):
    '''
    :param str filename: Filename under directory /resources/rig_editor/
    :returns: List of root widgets
    '''
    root_list = ror_gui.load_layout(filename, "", None)
    if len(root_list) is 0:
        raise Exception("gui_panels.load_gui_layout(): Error loading GUI layout [" + filename + "], no widgets found")
    return root_list
    
    
    
def cast_widget_type(widget, type_name):
    if   type_name is "TextBox":
        return widget.cast_type_TextBox()
    elif type_name is "ComboBox":
        return widget.cast_type_ComboBox()
    elif type_name is "EditBox":
        return widget.cast_type_EditBox()
    elif type_name is "Button":
        return widget.cast_type_Button()
    elif type_name is "MenuBar":
        return widget.cast_type_MenuBar()
    elif type_name is "PopupMenu":
        return widget.cast_type_PopupMenu()
    elif type_name is "Window":
        return widget.cast_type_Window()
    else:
        return widget        
    # TODO (not implemented yet): Checkbox                            



def bulk_find_child_widgets(root_widget, widget_types):
    '''
    :param str filename: Filename under directory /resources/rig_editor/
    :param dict widgets: [widget name -> widget type] map; Widgets are found and typecasted. Use 'None' to skip typecast.
    :returns:
    '''  
    if "_Root_" in widget_types:
        new_type = widget_types["_Root_"] 
        if new_type is not None:
            root_widget = cast_widget_type(root_widget, new_type)
             
    out_widgets = { "_Root_" : root_widget }
    for signature in widget_types.items():
        w_name = signature[0]
        w_type = signature[1]
        widget = root_widget.find_widget(w_name)
        if widget is None:
            raise Exception("gui_panels.bulk_find_child_widgets(): Widget with name [" + w_name + "] not found")
        out_widgets[w_name] = cast_widget_type(widget, w_type)
        
    return out_widgets
    
    
    
def load_single_root_layout(filename, widget_types):
     root_list = load_gui_layout(filename)
     return bulk_find_child_widgets(root_list[0], widget_types)   
    
        

def widget_bind_event_click(app, widget, ev_name):
    def fire_event(event_obj, sender_widget):
        event_obj.was_fired = True
    
    ev = app.events[ev_name]
    widget.event_mouse_button_click.add_delegate(ev, fire_event)
    
def widgets_bind_click_event(app, widgets_spec, widgets_obj):
    for widget_tup in widgets_spec.items():
        #print("widgets_bind_click_event(): TUP: ", widget_tup)
        if len(widget_tup[1]) >= 2:
            widget_name = widget_tup[0]
            widget_params = widget_tup[1]
            #print("widgets_bind_click_event(): PARAMS TUP: ", widget_params, ", ev_name:", widget_params[1])
            ev_name = widget_params[1]
            if ev_name not in app.events:
                raise Exception("gui_windows.widgets_bind_click_event():"
                    + " Attempt to bind non-existent event:", ev_name)
            widget_bind_event_click(app, widgets_obj[widget_name], ev_name)    
        
        

class WindowBase:

    def __init__(self, root_widget):
        self.root_widget = root_widget
        self.is_hidden_temporarily = False
        
    def show(self):
        self.is_hidden_temporarily = False
        self.root_widget.set_visible(True)
        
    def hide(self):
        self.is_hidden_temporarily = False
        self.root_widget.set_visible(False)
        
    def show_if_hidden_temporarily(self):
        if (self.is_hidden_temporarily):
            self.show()
            
    def hide_temporarily(self):
        self.is_hidden_temporarily = True
        self.root_widget.set_visible(False)
        
    def set_position(self, x_pixels, y_pixels):
        self.root_wdget.set_position(x_pixels, y_pixels)
        
    def center_to_screen(self):
        parent_size = self.root_widget.get_parent_size()
        this_size = self.root_widget.get_size()
        target_x = (parent_size.width  - this_size.width)  / 2
        target_y = (parent_size.height - this_size.height) / 2
        self.set_position(int(target_x), int(target_y))            
        
        

class Menubar(WindowBase):
    
    def __init__(self, app):
        # Load layout and widgets
        # dict {widget_name: (MyGUI widget type, on_click_event)}
        widgets = {
            "rig_editor_menubar": ("MenuBar", ), # Tuple!
            
            "mainmenu_item_load_project"    : ("MenuItem",                   ),
            "mainmenu_item_save_project_as" : ("MenuItem", "save_project_as" ),
            "mainmenu_item_close_project"   : ("MenuItem",                   ),
            "mainmenu_item_import_truckfile": ("MenuItem", "import_truckfile"),
            "mainmenu_item_export_truckfile": ("MenuItem",                   ),
            "mainmenu_item_quit_editor"     : ("MenuItem", "exit_rig_editor" )
        }
        self.widgets = load_single_root_layout("rig_editor_menubar.layout", widgets)
        WindowBase.__init__(self, self.widgets["rig_editor_menubar"])
        # Setup events
        widgets_bind_click_event(app, widgets, self.widgets)


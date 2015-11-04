
# Local
from euclid3 import Vector3
from datatypes import Color
import inputs
import camera
from demo import Demo

# System
import ror_system


class Event:
    'Named event. Can be bound to keyboard/mouse or invoked manually.'
    def __init__(self, name):
        self.name = name;
        self.was_fired = False;   



class Mode:
    'A named mode. Can be bound to keyboard/mouse or invoked manually.'
    def __init__(self, name):
        self.name = name
        self.was_activated = False
        self.was_deactivated = False
        self.is_active = False
        
    def activate(self):
        self.was_activated = True
        self.is_active = True
        
    def deactivate(self):
        self.was_deactivated = True
        self.is_active = False



class ConfigFile:

    def load(file_path):
        import json
        stream = open(file_path, "r")
        json_dict = json.load(stream)
        for section_name in json_dict:
            section = json_dict[section_name]
            for key in section:
                entry = section[key]
                # Fill default 
                print("entry type:", type(entry))
                if "value" not in entry: 
                    entry["value"] = entry["default_value"]
                if entry["value"] == None:
                    entry["value"] = entry["default_value"]
                # Convert
                datatype = entry["data_type"]
                if datatype == "RGB_Color":
                    entry["value"] = ConfigFile.convert_rgb(entry["value"])
        return json_dict
                
    def convert_rgb(rgb_list):
        return Color.from_rgb(rgb_list[0], rgb_list[1], rgb_list[2])               
                    
                

class Application:
    'RigEditor core'
    
    def __init__(self):
        print('Application initialized')
        self.was_exit_requested = False
        self.camera_controller = None
        import gui_manager
        self.gui_manager = gui_manager.GuiManager(self) 
        self.input_handler = inputs.InputListener()
        ror_system.register_input_listener(self.input_handler)
        self.events = {}
        self.modes = {}
        self.project_rig = None
        
        # TODO: obtain path from RoR
        file_path = "d:/Projects/Git/RoR-editor-python-dirty/"
        file_path += "bin/resources/skeleton/config/rig_editor_conf.json"
        self.config = ConfigFile.load(file_path)
        
        # Init events
        self.add_events([
            "exit_rig_editor",
            "create_empty_project",
            "close_project",
            "save_project_as",
            "load_project",
            "import_truckfile",
            "export_truckfile"
        ])
        
        # Init event mappings
        self.input_handler.add_key_down_event_mapping(inputs.KeyCodes.ESCAPE, self.events["exit_rig_editor"])        
        
    def add_event(self, name):
        ''' Defines a single event
            :returns: The :class:`Event` 
        '''
        e = Event(name)
        self.events[name] = e 
        return e
        
    def add_events(self, event_list):
        ''' Bulk-defines multiple events '''
        for e in event_list:
            self.add_event(e)
        
    def reset_events(self):
        for tup in self.events.items():
            event = tup[1]
            event.was_fired = False
        for tup in self.modes.items():
            mode = tup[1]
            mode.was_activated = False
            mode.was_deactivated = False 
        
    def _update_camera(self):
        mouse_state = self.input_handler.mouse_state
        do_orbit = mouse_state.is_right_button_pressed()
        self.camera_controller.inject_mouse_move(
            do_orbit, mouse_state.relative_x, mouse_state.relative_y, mouse_state.relative_z)
            
    def _on_event_import_truckfile(self):
        # TODO: Display open-file dialog
        
        # TEST
        import truckfile
        print("app: Importing truckfile")
        self.project_rig = truckfile.load(self.config, "d:/Projects/RoR/rig_editor_python", "import-test-rig.truck")
        print("app: Truckfile import done")
        self.project_rig.colorize_beams_default_scheme()
        self.project_rig.colorize_nodes_default_scheme()
        self.project_rig.update_beams_mesh()
        self.project_rig.update_node_meshes()
        
    def _on_event_save_project(self):
        # TODO: Display save-file dialog
        
        # TEST
        print("app: saving JSON project")
        import project_file
        project_file.save(self.project_rig, "d:/Projects/RoR/rig_editor_python", "rig-project-test.json.txt")
        print("app: JSON project saved")
        
    def _on_event_load_project(self):
        # TODO: Display open-file dialog
        
        # TEST
        print("app: ^loading JSON project")
        import project_file
        project_file.save(self.project_rig, "d:/Projects/RoR/rig_editor_python", "rig-project-test.json.txt")
        print("app: JSON project loaded")
    
    def go(self):
        ror_system.enter_rig_editor()
        Demo.draw_demo_mesh()
        self.camera_controller = camera.CameraOrbitController(
            ror_system.get_camera(), ortho_zoom_ratio=1.7)
        self.gui_manager.init_or_restore_gui()
        self.was_exit_requested = False
        
        # TEST
        
        self._on_event_import_truckfile()
        
        # END TEST
        
        while (not self.was_exit_requested):
            self.reset_events()
            self.input_handler.reset_inputs()
            ror_system.capture_input_and_update_gui()
            
            self._update_camera()
            
            if (self.events["exit_rig_editor"].was_fired):
                self.was_exit_requested = True
                
            if (self.events["import_truckfile"].was_fired):
                self._on_event_import_truckfile()
                
            if (self.events["save_project_as"].was_fired):
                self._on_event_save_project()
                
            if (self.events["load_project"].was_fired):
                self._on_event_load_project()
        
            if (ror_system.is_application_window_closed()):
                ror_system.request_application_shutdown()
                self.was_exit_requested = True
                
            ror_system.render_frame_and_update_window()
            
        self.was_exit_requested = False
        self.gui_manager.temporarily_hide_gui()
        
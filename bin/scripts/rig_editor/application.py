
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



class Application:
    'RigEditor core'
    
    def __init__(self):
        print('Application initialized')
        self.was_exit_requested = False
        self.camera_controller = None
        self.input_handler = inputs.InputListener()
        ror_system.register_input_listener(self.input_handler)
        self.events = {}
        self.modes = {}
        self.__init_events()
        self.__init_event_mappings()
        
    def add_event(self, name):
        self.events[name] = Event(name)
        
    def __init_events(self):
        self.add_event("exit_rig_editor")
        
    def __init_event_mappings(self):
        self.input_handler.add_key_down_event_mapping(inputs.KeyCodes.ESCAPE, self.events["exit_rig_editor"])
        
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
    
    def go(self):
        ror_system.enter_rig_editor()
        Demo.draw_demo_mesh()
        self.camera_controller = camera.CameraOrbitController(
            ror_system.get_camera(), ortho_zoom_ratio=1.7)
        self.was_exit_requested = False
        
        # Truckfile import test
        Demo.test_truckfile_import("d:\Projects\Rigs of Rods\RigEditor-Python", "test-rig.truck")
        
        while (not self.was_exit_requested):
            self.reset_events()
            self.input_handler.reset_inputs()
            ror_system.capture_input_and_update_gui()
            
            self._update_camera()
            
            if (self.events["exit_rig_editor"].was_fired):
                self.was_exit_requested = True
        
            if (ror_system.is_application_window_closed()):
                ror_system.request_application_shutdown()
                self.was_exit_requested = True
                
            ror_system.render_frame_and_update_window()
            
        self.was_exit_requested = False
        
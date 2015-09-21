
# Local
from datatypes import Vector3, Color
import inputs

# System
import ror_system
import ror_drawing



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
        
    def draw_demo_mesh(self):
        mesh = ror_drawing.create_lines_mesh();
        mesh.set_position(Vector3(0,0,0))
        mesh.begin_update()
        mesh.add_line(Vector3(0, 0, 0), Color(1, 1 ,0),     Vector3(5, 0, 0), Color(1, 0, 0))
        mesh.add_line(Vector3(0, 0, 0), Color(0.2, 0.8 ,1), Vector3(0, 0, 5), Color(0, 0, 1))
        mesh.add_line(Vector3(0, 0, 0), Color(0.1, 1, 0.7), Vector3(0, 5, 0), Color(0, 1, 0))
        mesh.end_update()
        mesh.attach_to_scene()
    
    def go(self):
        ror_system.enter_rig_editor()
        self.draw_demo_mesh()
        self.was_exit_requested = False
        
        while (not self.was_exit_requested):
            self.reset_events()
            ror_system.capture_input_and_update_gui()
            
            if (self.events["exit_rig_editor"].was_fired):
                self.was_exit_requested = True
        
            if (ror_system.is_application_window_closed()):
                ror_system.request_application_shutdown()
                self.was_exit_requested = True
                
            ror_system.render_frame_and_update_window()
            
        self.was_exit_requested = False
        
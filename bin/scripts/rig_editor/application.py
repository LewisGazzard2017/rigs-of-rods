
# Local
from datatypes import Vector3, Color
import input

# System
import ror_system
import ror_drawing


class Application:
    'RigEditor core'
    
    def __init__(self):
        print('Application initialized')
        self.was_exit_requested = False
        self.input_handler = input.InputListener()
        ror_system.register_input_listener(self.input_handler)
        
    def draw_demo_mesh(self):
        mesh = ror_drawing.create_lines_mesh();
        mesh.set_position(Vector3(0,0,0))
        mesh.begin_update()
        mesh.add_line(Vector3(0,0,0), Color(1,0.5,0), Vector3(1,0,0), Color(1,0,0))
        mesh.add_line(Vector3(0,0,0), Color(0,0.5,1), Vector3(0,0,1), Color(0,0,1))
        mesh.add_line(Vector3(0,0,0), Color(0,1,0.5), Vector3(0,1,0), Color(0,1,0))
        mesh.end_update()
        mesh.attach_to_scene()
    
    def go(self):
        ror_system.enter_rig_editor()
        self.draw_demo_mesh()
        self.was_exit_requested = False
        
        while (not self.was_exit_requested):
            ror_system.capture_input_and_update_gui()
            
            if (self.input_handler.events["exit_rig_editor"].was_fired):
                self.was_exit_requested = True
        
            if (ror_system.is_application_window_closed()):
                ror_system.request_application_shutdown()
                self.was_exit_requested = True
                
            ror_system.render_frame_and_update_window()
            
        self.was_exit_requested = False
        
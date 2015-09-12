
# ==============================================================================
# Rigs of Rods - Rig Editor
# Main script
# ==============================================================================

print('Hello, Rig editor here!')

# ------------------------------------------------------------------------------

class Vector3:
    '3D vector'
    x = 0
    y = 0
    z = 0
    
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z  

# ------------------------------------------------------------------------------

class Color:
    'RGBA color'
    r = 0;
    g = 0;
    b = 0;
    a = 1;
    
    def __init__(self, r, g, b, a):
        self.r = r;
        self.g = g;
        self.b = b;
        self.a = a;
        
    def __init__(self, r, g, b):
        self.r = r;
        self.g = g;
        self.b = b;
        self.a = 1;

# ------------------------------------------------------------------------------   

# Print test output
# print('DBG LINE ')

import ror_system;
import ror_drawing;
print ('DBG ror_system, ror_drawing imported');

ror_system.enter_rig_editor();
print('DBG LINE ror_system.enter_rig_editor();')

mesh = ror_drawing.create_lines_mesh();
print('DBG LINE ror_drawing.create_line_mesh();')

mesh.set_position(Vector3(0,0,0))
print('DBG LINE mesh.set_position(Vector3(1,2,3))')

mesh.begin_update()
print('DBG LINE begin_update()');

mesh.add_line(Vector3(0,0,0), Color(1,0.5,0), Vector3(10,0,0), Color(1,0,0))
print('DBG LINE 1')

mesh.add_line(Vector3(0,0,0), Color(0,0.5,1), Vector3(0,0,10), Color(0,0,1))
print('DBG LINE 2')

mesh.add_line(Vector3(0,0,0), Color(0,1,0.5), Vector3(0,10,0), Color(0,1,0))
print('DBG LINE 3')

mesh.end_update()
print('DBG LINE end_update()');

mesh.attach_to_scene()
print('DBG LINE attach_to_scene()');

ror_system.render_frame_and_update_window();
print('DBG LINE ror_system.render_frame_and_update_window();');

import time
time.sleep(5)

print('Goodbye, Rig editor exits.')

# Exit 

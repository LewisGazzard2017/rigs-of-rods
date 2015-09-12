
# ==============================================================================
# Rigs of Rods - Rig Editor
# Main script
# ==============================================================================

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

# Print test output
print('Hello, Rig editor here!')

import ror_system;
print ('DBG ror_system imported');

import ror_drawing;
print ('DBG ror_drawing imported');

ror_system.enter_rig_editor();
print('DBG LINE ror_system.enter_rig_editor();')



mesh = ror_drawing.create_line_mesh();
print('DBG LINE ror_drawing.create_line_mesh();')

mesh.set_position(Vector3(1,2,3))
print('DBG LINE mesh.set_position(Vector3(1,2,3))')

ror_system.render_frame_and_update_window();
print('DBG LINE ror_system.render_frame_and_update_window();');

import time
time.sleep(5)

print('Goodbye, Rig editor exits.')

# Exit 

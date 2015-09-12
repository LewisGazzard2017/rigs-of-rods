
# ==============================================================================
# Rigs of Rods - Rig Editor
# Main script
# ==============================================================================

# Print test output
print('Hello, Rig editor here!')

import ror_system;
print ('DBG ror_system imported');

ror_system.enter_rig_editor();
print('DBG LINE ror_system.enter_rig_editor();')

ror_system.render_frame_and_update_window();
print('DBG LINE ror_system.render_frame_and_update_window();');

import time
time.sleep(5)

print('Goodbye, Rig editor exits.')

# Exit 

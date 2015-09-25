:mod:`ror_system` --- Interface to hosting application
======================================================

.. module:: ror_system
   :synopsis: Interface to hosting application.

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

This module enables RigEditor to communicate with it's host application.
It contains only necessary minimum of functionality, such as receiving input,
requesting a frame to be rendered or application to be terminated.

.. note::

   Functionality provided by this module is considered internal. Most
   developers should never need to access it directly. RigEditor provides
   higher level controls through it's own modules.

Entering and exiting RigEditor
------------------------------

.. function:: enter_rig_editor()

   Performs application-side initialization necessary to run RigEditor.
   Without calling this function at the beginning of the program,
   results of all other operations are undefined.
   Must be called only once, multiple calls -> undefined.
   
.. function:: request_application_shutdown()

   Requests the host application to exit. More specifically, signal the host 
   application to exit it's main loop, as soon as control returns to it (i.e
   as soon as RigEditor's main loop exits). This call doesn't terminate
   RigEditor's main loop, nor does it terminate the Python interpreter. 

Input handling
--------------

.. note::

   This is low-level functionality. Use module :mod:`inputs` for managing
   input events.
   
.. function:: register_input_listener(input_listener)

   On the low level, RigEditor receives input by defining an 'input listener'
   class with callback methods for specific input events.
   
   The underlying input handling library is OIS.
   
   **Keyboard event callbacks**::
   
       key_pressed_callback (ois_key_code, unicode_character)
       key_released_callback(ois_key_code, unicode_character)
   
   **Mouse event callbacks**::
   
       mouse_moved_or_scrolled_callback(x_abs, y_abs, x_rel, y_rel, wheel_rel)
       
       mouse_pressed_callback (ois_button_code, x_abs, y_abs)
       mouse_released_callback(ois_button_code, x_abs, y_abs)
   

.. function:: capture_input_and_update_gui()

   Processes all input events from underlying operating system, invoking
   respective callbacks from 'input listener'. Each event is injected to GUI
   subsystem (MyGUI library) first and if the GUI reports the event as 
   'handled', the handling is terminated ('input listener' callback is
   not invoked).
   
   See documentation of MyGUI input-injecting API for details:
   [TODO]

Camera control
--------------

.. note::

   This is low-level functionality. Use module :mod:`camera` for camera handling.

.. function:: get_camera()

   Obtains the camera object from underlying 3D engine.
   
.. class:: Camera   

   .. method:: look_at(position)                         
   .. method:: yaw_degrees(angle)                     
   .. method:: roll_degrees(angle)                    
   .. method:: set_position(position)                    
   .. method:: pitch_degrees(angle)                   
   .. method:: is_mode_ortho                   
   .. method:: move_relative(offset)                   
   .. method:: set_mode_ortho(be_ortho)                  
   .. method:: toggle_mode_ortho               
   .. method:: set_fov_y_degrees(angle)               
   .. method:: get_point_z_distance(position)            
   .. method:: set_far_clip_distance(distance)           
   .. method:: set_near_clip_distance(distance)          
   .. method:: set_ortho_window_width(width)          
   .. method:: convert_world_to_screen_position
   .. method:: convert_screen_to_world_position

Output handling
---------------

.. function:: render_frame_and_update_window()

   Requests the underlying 3d engine (OGRE) to render one frame and update
   application window. The call is synchronous - it blocks execution until
   rendering is done.
   
.. function:: is_application_window_closed()

   Returns True if the application's window was closed using OS controls
   (such as [X] button on the window's top bar).
:mod:`camera` --- Camera handling
=================================

.. module:: camera
   :synopsis: Camera handling

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

Provides automated camera handling, suitable for editor.

.. class:: CameraOrbitController(system_camera, ortho_zoom_ratio)

   Mouse-controlled camera orbiting around a defined point.
   
   Parameter #1 "system_camera" must be obtained from :mod:`ror_system`
   
   Parameter #2 "ortho_zoom_ratio" (real number) is a ratio between camera 
   distance from target and orthographic "window size" which simulates zooming.
   
   .. method:: update_ortho_zoom()
      
      Re-calculates and updates orthographic "zoom" (virtual window size)
      
   .. method:: inject_mouse_move(do_orbit, x_rel, y_rel, wheel_rel)
   
      Param #1 "do_orbit": True to interpret mouse motion as camera movement.
      
      Param #2 "x_rel": Number: mouse X movement since last update.
      
      Param #3 "y_rel": Number: mouse Y movement since last update.
      
      Param #4 "wheel_rel": Number: mouse wheel movement since last update.
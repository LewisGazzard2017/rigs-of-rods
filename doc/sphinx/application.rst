:mod:`application` --- Main application class
=============================================

.. module:: application
   :synopsis: Main application class.

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

The heart of RigEditor. Defines application-wide Events and Modes.
Implements and runs main loop.

Events and modes
----------------

Design principles:

* The programmer should ask "did the user press a 'grab' hotkey?" rather
  than "did the user press 'G' key?". This enables clean configuration.
* The same event may be invoked by both input device (hotkey...) and
  different way (action menu...). So the best thing
  for the programmer to ask is "was 'grab mode' invoked?".
* Events should not be queued for one-by-one processing, nor invoke callbacks
  to directly modify the environment. Instead, they should be all available
  for random lookup so that the programmer can decide the order of processing
  them in main loop and easily track dependencies. 
  
Modes/Events as implemented here are practically just labels in a state table.
They carry state information but no data related to their purpose.

.. class:: Event
   
   Named event. Can be bound to keyboard/mouse or invoked manually.
   
   .. attribute:: name
   
   String.
   
   .. attribute:: was_fired
   
   (Boolean) Was event activated in this iteration? Resets on new iteration.
   
   

.. class:: Mode

   A named mode. Can be bound to keyboard/mouse or invoked manually.

   .. attribute:: name
   
   String.
   
   .. attribute:: was_activated
   
   (Boolean) Was mode activated in this iteration? Resets on new iteration.
   
   .. attribute:: was_deactivated
   
   (Boolean) Was mode deactivated in this iteration? Resets on new iteration.
   
   .. attribute:: is_active
   
   (Boolean) Is mode active? Doesn't reset.             
      
   .. method:: activate
      
   .. method:: deactivate



The application class
---------------------

.. class:: Application:
   
   RigEditor core
   
   .. attribute:: camera_controller
   
      Instance of :class:`camera.CameraOrbitController`
   
   .. attribute:: input_handler
   
      Instance of :class:`inputs.InputListener`
   
   .. attribute:: events
   
      Dictionary {name -> object} of all :class:`Event` instances available.
   
   .. attribute:: modes
   
      Dictionary {name -> object} of all :class:`Mode` instances available.      
      
   .. method:: add_event(name)
      
      Creates and returns new :class:`Event` instance.
      
   .. method:: go()
   
      Enters main loop.      
      
   **Internal:** 
      
   .. attribute:: was_exit_requested
   
      Boolean. Internal.      

   .. method:: reset_events()
      
   .. method:: __init_events()
      
   .. method:: __init_event_mappings()
   
   .. method:: _update_camera()
   
      Transfer mouse inputs to camera.
      
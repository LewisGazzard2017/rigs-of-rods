:mod:`ror_truckfile` --- Loading and saving files in "truck" format
===================================================================

.. module:: ror_truckfile
   :synopsis: Loading and saving files in "truck" format

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>

This module provides utilities to create/save/load/process files in 
RoR's "truck" format, called truckfiles. 
See http://www.rigsofrods.com/wiki/pages/Truck_Description_File
for reference.

Rigs are represented as trees of data structures.
These trees are either obtained by parsing a truckfile or constructed in code.

The philosophy of this module is to only load the data and
present it as-is, using only minimum of transformation.

.. note::

   This module is under development. Some functionality described here
   may not work or may be missing for the moment.
   

Loading and saving truckfiles
-----------------------------

.. class:: Parser()

   Parses and validates a truckfile 
   as tree of data structures contained in
   :class:`File` object.
   
   .. method:: parse_file(directory, filename)
   
   Parses and validates a truckfile.
   
   Does not return any value. Results must be obtained using other methods.
   
   .. method:: get_parsed_file()
   
   Returns the generated :class:`File` object.
   
   .. note::
      This method passes ownership of the object.
      Subsequent calls will return None!
      

File, Modules and properties
----------------------------

The root container is :class:`File` which defines global attributes,
such as the name of the rig.

All non-global elements and attributes are contained in 
:class:`Module`-s. These are analogous to "sectionconfig" directive
in truckfile syntax. Elements without explicitly defined "sectionconfig"
are put into "root module".



.. class:: File()

   Root element of data tree.
   
   .. attribute:: name
   
   Name of the rig
   
   .. attribute:: root_module
   
   Default :class:`Module`. Always present. It's name is "_Root_".
   
   .. attribute:: modules
   
   Dictionary of explicitly defined :class:`Module`-s.
   The root module is not included.
   
   

.. class:: Module()

   Container of non-global truckfile elements. Multiple modules can
   be created using "sectionconfig" directive.

   .. attribute:: name
       
   Name of the module
      
   .. attribute:: nodes_by_preset
   
   List of :class:`NodeGroupWithPreset` objects.
   
   .. attribute:: beams_by_preset
   
   List of :class:`BeamGroupWithPreset` objects.
   
   .. attribute:: command_hydros_by_preset
   
   List of :class:`CommandHydroGroupWithPreset` objects.
   
   
   
Physics body
------------

   RoR uses a spring-mass-damp model: The physics body
   (called Rig in RoR jargon) 
   contains
   of weighted, infinitely small points (called Nodes in RoR jargon)
   and weightless, springing and dampening edges (Beams in RoR jargon).
   
   In the truckfile, nodes have assigned IDs and beams reference them
   via these IDs. Since RoR 0.4.5, IDs are always strings (text).
   However, RoR has a long history of exclusively number-IDs which had
   to be ordered ascendingly (1,2,3,4...) in the truckfile. 
   Later, 'named nodes' were introduced, but the implementation was hybrid:
   under the hood, named nodes were auto-assigned a number and content
   creators were free to access named nodes through the auto-assigned numbers.
   
   To implement the ID + Reference model with all required backwards
   compatibility, both the :class:`NodeId` and :class:`NodeIdRef` are
   opaque objects. The parser detects pre-v0.4.5 truckfiles
   (fileformatversion < 450) and automatically converts them to
   ID + Ref representation.
   
   
   
.. class:: NodeId()

   .. method:: to_str()
   
   Converts the ID to str in format "{id} {flags: NUMBERED/NAMED...}"
   
   

.. class:: NodeRef()

   TODO

   
   
.. class:: Node()

   Elementary building block of Rig.
   
   .. attribute:: id
   
   Instance of :class:`NodeId`. Uniquely identifies this Node.
   
   .. attribute:: position

   Vector3.

   .. attribute:: load_weight_override
   
   Float.
   
   .. attribute:: _has_load_weight_override
   
   Boolean: is "load_weight_override" specified in truckfile?
   
   .. attribute:: node_defaults
   
   Instance of :class:`NodeDefaults`
   
   .. attribute:: beam_defaults
   
   Instance of :class:`BeamDefaults`
   
   .. attribute:: detacher_group
   
   Integer: Identifier of detacher group.
   
   **Options (boolean)**

   .. attribute:: option_n
   .. attribute:: option_m
   .. attribute:: option_f
   .. attribute:: option_x
   .. attribute:: option_y
   .. attribute:: option_c
   .. attribute:: option_h
   .. attribute:: option_e
   .. attribute:: option_b
   .. attribute:: option_p
   .. attribute:: option_L
   .. attribute:: option_l
   


.. class:: NodePreset()

   Node preset, corresponds to "set_node_defaults" in truckfile.
   
   .. attribute:: load_weight
   
   Float.
   
   .. attribute:: friction
   
   Float.
   
   .. attribute:: volume
   
   Float
   
   .. attribute:: surface
   
   Float
   
   **Options (boolean)**

   .. attribute:: option_n
   .. attribute:: option_m
   .. attribute:: option_f
   .. attribute:: option_x
   .. attribute:: option_y
   .. attribute:: option_c
   .. attribute:: option_h
   .. attribute:: option_e
   .. attribute:: option_b
   .. attribute:: option_p
   .. attribute:: option_L
   .. attribute:: option_l 
   
   
   
.. class:: NodeGroupWithPreset() 

   .. attribute:: nodes
   
   List of :class:`Node`
   
   .. attribute:: preset
   
   Instance of :class:`NodePreset`             



.. class:: Beam()

   TODO
   
   
.. class:: BeamPreset()

   Corresponds to "set_beam_defaults" in truckfile.
   


.. class:: BeamGroupWithPreset() 

   .. attribute:: beams
   
   List of :class:`Beam`
   
   .. attribute:: preset
   
   Instance of :class:`BeamPreset`
   
   
.. class:: CommandHydro()

   Special beam. Hydraulics responding to user input.
   
   .. attribute:: shorten_rate
   .. attribute:: lengthen_rate
   .. attribute:: max_contraction
   .. attribute:: max_extension
   .. attribute:: contract_key
   .. attribute:: extend_key
   .. attribute:: description
   .. attribute:: affect_engine
   .. attribute:: needs_engine
   
   .. attribute:: detacher_group
   .. attribute:: beam_preset
   .. attribute:: inertia_preset
   
   .. attribute:: option_r_rope             
   .. attribute:: option_c_auto_center      
   .. attribute:: option_f_not_faster       
   .. attribute:: option_p_press_once       
   .. attribute:: option_o_press_once_center
   
.. class:: BeamGroupWithPreset() 

   .. attribute:: command_hydros
   
   List of :class:`CommandHydro`
   
   .. attribute:: preset
   
   Instance of :class:`BeamPreset`
   
   
   
Aerial
------

      
.. class:: WingControlSurface

   Constants

   .. attribute:: n_NONE                 
   .. attribute:: a_RIGHT_AILERON        
   .. attribute:: b_LEFT_AILERON         
   .. attribute:: f_FLAP                 
   .. attribute:: e_ELEVATOR             
   .. attribute:: r_RUDDER               
   .. attribute:: S_RIGHT_HAND_STABILATOR
   .. attribute:: T_LEFT_HAND_STABILATOR 
   .. attribute:: c_RIGHT_ELEVON         
   .. attribute:: d_LEFT_ELEVON          
   .. attribute:: g_RIGHT_FLAPERON       
   .. attribute:: h_LEFT_FLAPERON        
   .. attribute:: U_RIGHT_HAND_TAILERON  
   .. attribute:: V_LEFT_HAND_TAILERON   
   .. attribute:: i_RIGHT_RUDDERVATOR    
   .. attribute:: j_LEFT_RUDDERVATOR     
   .. attribute:: INVALID                




.. class:: Wing

   .. attribute:: control_surface
   .. attribute:: chord_point    
   .. attribute:: min_deflection 
   .. attribute:: max_deflection 
   .. attribute:: airfoil_name   
   .. attribute:: efficacy_coef  
        
   .. method:: get_node(index)

      Index must be from interval (0, 7)

   .. method:: set_node(index, node_ref)

      Index must be from interval (0, 7)

   .. method:: get_texcoord(index)

      Index must be from interval (0, 3)

   .. method:: set_texcoord(index, x, y)

      Index must be from interval (0, 3); X/Y are U/V texture coordinates (0 - 1)




.. class:: Airbrake
    
   .. attribute:: reference_node       
   .. attribute:: x_axis_node          
   .. attribute:: y_axis_node          
   .. attribute:: aditional_node       
   .. attribute:: offset               
   .. attribute:: width                
   .. attribute:: height               
   .. attribute:: max_inclination_angle
   .. attribute:: texcoord_x1          
   .. attribute:: texcoord_x2          
   .. attribute:: texcoord_y1          
   .. attribute:: texcoord_y2          
   .. attribute:: lift_coefficient     




.. class:: Turbojet
   
   .. attribute:: front_node     
   .. attribute:: back_node      
   .. attribute:: side_node      
   .. attribute:: is_reversable  
   .. attribute:: dry_thrust     
   .. attribute:: wet_thrust     
   .. attribute:: front_diameter 
   .. attribute:: back_diameter  
   .. attribute:: nozzle_length  




.. class:: Turboprop

   .. method:: get_blade_tip_node(index)

      Index must be from interval (0, 7)

   .. method:: set_blade_tip_node(index, node_ref)

      Index must be from interval (0, 7)

   .. attribute:: reference_node   
   .. attribute:: axis_node        
   .. attribute:: turbine_power_kW 
   .. attribute:: airfoil          
   .. attribute:: couple_node      
   .. attribute:: _format_version  




.. class:: Pistonprop

   .. method:: get_blade_tip_node(index)

      Index must be from interval (0, 7)

   .. method:: set_blade_tip_node(index, node_ref)

      Index must be from interval (0, 7)

   .. attribute:: reference_node  
   .. attribute:: axis_node        
   .. attribute:: couple_node      
   .. attribute:: _couple_node_set 
   .. attribute:: turbine_power_kW 
   .. attribute:: pitch            
   .. attribute:: airfoil          




.. class:: Fusedrag

   .. method:: use_autocalc       
   .. method:: front_node         
   .. method:: rear_node          
   .. method:: approximate_width  
   .. method:: airfoil_name       
   .. method:: area_coefficient   

 
   
Powertrain
----------


.. class:: InertiaPreset
        
   .. attribute:: start_delay_factor
   .. attribute:: stop_delay_factor 
   .. attribute:: start_function    
   .. attribute:: stop_function     
        


.. class:: OptionalInertia
        
   .. attribute:: start_delay_factor     
   .. attribute:: stop_delay_factor      
   .. attribute:: start_function         
   .. attribute:: stop_function          
   .. attribute:: _start_delay_factor_set
   .. attribute:: _stop_delay_factor_set 
        


.. class:: Engine
        
   .. attribute:: shift_down_rpm      
   .. attribute:: shift_up_rpm        
   .. attribute:: torque              
   .. attribute:: global_gear_ratio   
   .. attribute:: reverse_gear_ratio  
   .. attribute:: neutral_gear_ratio  
   .. attribute:: gear_ratios         
        


.. class:: Axle        

   .. attribute:: wheel1_node1
   .. attribute:: wheel1_node2
   .. attribute:: wheel2_node1
   .. attribute:: wheel2_node2

   .. attribute:: options

   Vector of characters; each char is an option. Order matters. Options can repeat.

        

.. class:: Engoption
        
   .. attribute:: inertia                    
   .. attribute:: type                       
   .. attribute:: clutch_force               
   .. attribute:: _clutch_force_use_default  
   .. attribute:: shift_time                 
   .. attribute:: clutch_time                
   .. attribute:: post_shift_time            
   .. attribute:: idle_rpm                   
   .. attribute:: _idle_rpm_use_default      
   .. attribute:: stall_rpm                  
   .. attribute:: max_idle_mixture           
   .. attribute:: min_idle_mixture           
     
        

.. class:: TorqueCurveSample
        
   .. attribute:: power                 
   .. attribute:: torque_percent        
       
        

.. class:: TorqueCurve
        
   .. attribute:: samples               
   .. attribute:: predefined_func_name
   
   
   
.. class:: CruiseControl      
            
   .. attribute:: min_speed
   .. attribute:: autobrake



.. class:: Brakes              
    
   .. attribute:: default_braking_force   
   .. attribute:: parking_brake_force     
   .. attribute:: _was_parking_brake_force_defined
   
      Informs whether the optional attribute "parking_brake_force" was specified.



.. class:: AntiLockBrakes    
              
   .. attribute:: has_mode_on          
   .. attribute:: has_mode_off         
   .. attribute:: has_mode_no_dashboard
   .. attribute:: has_mode_no_togle    

   .. attribute:: regulation_force  
   .. attribute:: min_speed         
   .. attribute:: pulse_per_sec     
   .. attribute:: _was_pulse_per_sec_defined
   
      Informs whether the optional attribute "pulse_per_sec" was specified.



.. class:: SlopeBrake         
     
   .. attribute:: regulating_force
   .. attribute:: attach_angle     
   .. attribute:: release_angle    



.. class:: TractionControl                  

   .. attribute:: has_mode_on          
   .. attribute:: has_mode_off         
   .. attribute:: has_mode_no_dashboard
   .. attribute:: has_mode_no_togle    

   .. attribute:: regulation_force
   .. attribute:: wheel_slip       
   .. attribute:: fade_speed       
   .. attribute:: pulse_per_sec         
        

Wheels
------


.. class:: Wheel

   .. attribute:: node1
   .. attribute:: node2
   .. attribute:: width             
   .. attribute:: num_rays          
   .. attribute:: rigidity_node     
   .. attribute:: braking           
   .. attribute:: propulsion        
   .. attribute:: reference_arm_node
   .. attribute:: mass              
   .. attribute:: node_preset       
   .. attribute:: beam_preset       
   .. attribute:: radius            
   .. attribute:: springiness       
   .. attribute:: damping           
   .. attribute:: face_material_name



.. class:: Wheel2

   .. attribute:: node1
   .. attribute:: node2
   .. attribute:: rim_radius       
   .. attribute:: tyre_radius      
   .. attribute:: tyre_springiness 
   .. attribute:: tyre_damping     
   .. attribute:: face_material_name
   .. attribute:: band_material_name
   .. attribute:: rim_springiness   
   .. attribute:: rim_damping       



.. class:: MeshWheel

   .. attribute:: node1
   .. attribute:: node2
   .. attribute:: width             
   .. attribute:: num_rays          
   .. attribute:: rigidity_node     
   .. attribute:: braking           
   .. attribute:: propulsion        
   .. attribute:: reference_arm_node
   .. attribute:: mass              
   .. attribute:: node_preset       
   .. attribute:: beam_preset       
   .. attribute:: side           
   .. attribute:: mesh_name      
   .. attribute:: material_name  
   .. attribute:: rim_radius     
   .. attribute:: tyre_radius    
   .. attribute:: spring         
   .. attribute:: damping        



.. class:: MeshWheel2

   .. attribute:: node1
   .. attribute:: node2
   .. attribute:: rim_radius       
   .. attribute:: tyre_radius      
   .. attribute:: tyre_springiness 
   .. attribute:: tyre_damping     
   .. attribute:: side           
   .. attribute:: mesh_name      
   .. attribute:: material_name  



.. class:: FlexBodyWheel

   .. attribute:: node1
   .. attribute:: node2
   .. attribute:: rim_radius       
   .. attribute:: tyre_radius      
   .. attribute:: tyre_springiness 
   .. attribute:: tyre_damping     
   .. attribute:: side            
   .. attribute:: rim_springiness 
   .. attribute:: rim_damping     
   .. attribute:: rim_mesh_name   
   .. attribute:: tyre_mesh_name  


Project status
--------------

   List of all available truckfile keywords with comments on availability
   through Python.

   * KEYWORD_ADD_ANIMATION
   * KEYWORD_AIRBRAKES                  ~ :class:`Airbrake`
   * KEYWORD_ANIMATORS
   * KEYWORD_ANTI_LOCK_BRAKES
   * KEYWORD_AXLES                      ~ :class:`Axle`
   * KEYWORD_AUTHOR
   * KEYWORD_BACKMESH
   * KEYWORD_BEAMS                      ~ :class:`Beam`
   * KEYWORD_BRAKES
   * KEYWORD_CAB
   * KEYWORD_CAMERARAIL
   * KEYWORD_CAMERAS
   * KEYWORD_CINECAM
   * KEYWORD_COLLISIONBOXES
   * KEYWORD_COMMANDS
   * KEYWORD_COMMANDS2
   * KEYWORD_CONTACTERS
   * KEYWORD_CRUISECONTROL
   * KEYWORD_DESCRIPTION
   * KEYWORD_DETACHER_GROUP
   * KEYWORD_DISABLEDEFAULTSOUNDS
   * KEYWORD_ENABLE_ADVANCED_DEFORMATION
   * KEYWORD_END
   * KEYWORD_END_SECTION
   * KEYWORD_ENGINE                      ~ :class:`Engine`
   * KEYWORD_ENGOPTION                   ~ :class:`Engoption`
   * KEYWORD_ENVMAP
   * KEYWORD_EXHAUSTS
   * KEYWORD_EXTCAMERA
   * KEYWORD_FILEFORMATVERSION
   * KEYWORD_FILEINFO
   * KEYWORD_FIXES
   * KEYWORD_FLARES
   * KEYWORD_FLARES2
   * KEYWORD_FLEXBODIES
   * KEYWORD_FLEXBODY_CAMERA_MODE
   * KEYWORD_FLEXBODYWHEELS               ~ :class:`FlexBodyWheel`
   * KEYWORD_FORWARDCOMMANDS
   * KEYWORD_FUSEDRAG                     ~ :class:`Fusedrag`
   * KEYWORD_GLOBALS
   * KEYWORD_GUID
   * KEYWORD_GUISETTINGS
   * KEYWORD_HELP
   * KEYWORD_HIDE_IN_CHOOSER
   * KEYWORD_HOOKGROUP
   * KEYWORD_HOOKS
   * KEYWORD_HYDROS
   * KEYWORD_IMPORTCOMMANDS
   * KEYWORD_LOCKGROUPS
   * KEYWORD_LOCKGROUP_DEFAULT_NOLOCK
   * KEYWORD_MANAGEDMATERIALS
   * KEYWORD_MATERIALFLAREBINDINGS
   * KEYWORD_MESHWHEELS                  ~ :class:`MeshWheel`
   * KEYWORD_MESHWHEELS2                 ~ :class:`MeshWheel2`
   * KEYWORD_MINIMASS
   * KEYWORD_NODECOLLISION
   * KEYWORD_NODES
   * KEYWORD_NODES2
   * KEYWORD_PARTICLES
   * KEYWORD_PISTONPROPS                  ~ :class:`Pistonprop`
   * KEYWORD_PROP_CAMERA_MODE
   * KEYWORD_PROPS
   * KEYWORD_RAILGROUPS
   * KEYWORD_RESCUER
   * KEYWORD_RIGIDIFIERS
   * KEYWORD_ROLLON
   * KEYWORD_ROPABLES
   * KEYWORD_ROPES
   * KEYWORD_ROTATORS
   * KEYWORD_ROTATORS2
   * KEYWORD_SCREWPROPS
   * KEYWORD_SECTION                      ~ :class:`Module`
   * KEYWORD_SECTIONCONFIG                ~ :class:`Module`
   * KEYWORD_SET_BEAM_DEFAULTS            ~ :class:`BeamPreset`
   * KEYWORD_SET_BEAM_DEFAULTS_SCALE      ~ :class:`BeamPreset`
   * KEYWORD_SET_COLLISION_RANGE
   * KEYWORD_SET_INERTIA_DEFAULTS         ~ :class:`InertiaPreset`
   * KEYWORD_SET_MANAGEDMATERIALS_OPTIONS
   * KEYWORD_SET_NODE_DEFAULTS            ~ :class:`NodePreset`
   * KEYWORD_SET_SHADOWS
   * KEYWORD_SET_SKELETON_SETTINGS
   * KEYWORD_SHOCKS
   * KEYWORD_SHOCKS2
   * KEYWORD_SLIDENODE_CONNECT_INSTANTLY
   * KEYWORD_SLIDENODES
   * KEYWORD_SLOPE_BRAKE
   * KEYWORD_SOUNDSOURCES
   * KEYWORD_SOUNDSOURCES2
   * KEYWORD_SPEEDLIMITER
   * KEYWORD_SUBMESH
   * KEYWORD_SUBMESH_GROUNDMODEL
   * KEYWORD_TEXCOORDS
   * KEYWORD_TIES
   * KEYWORD_TORQUECURVE                  ~ :class:`TorqueCurve`
   * KEYWORD_TRACTION_CONTROL
   * KEYWORD_TRIGGERS
   * KEYWORD_TURBOJETS                    ~ :class:`Turbojet`
   * KEYWORD_TURBOPROPS                   ~ :class:`Turboprop`
   * KEYWORD_TURBOPROPS2                  ~ :class:`Turboprop`
   * KEYWORD_VIDEOCAMERA
   * KEYWORD_WHEELS                       ~ :class:`Wheel`
   * KEYWORD_WHEELS2                      ~ :class:`Wheel2`
   * KEYWORD_WINGS                        ~ :class:`Wing`


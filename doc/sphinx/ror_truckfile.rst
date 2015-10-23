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

================================  ==============================
Truckfile keyword                 :mod:`ror_truckfile` class
================================  ==============================
add_animation                     :class:`Animation`
airbrakes                         :class:`Airbrake`
animators                         :class:`Animator`
anti_lock_brakes                  :class:`AntiLockBrake`
axles                             :class:`Axle`
author                            :class:`Author`
backmesh                          Attribute of :class:`Submesh`
beams                             :class:`Beam`
brakes                            :class:`Brake`
cab                               :class:`CabTriangle`
camerarail                        :class:`CameraRail`
cameras                           :class:`Camera`
cinecam                           :class:`Cinecam`
collisionboxes                    :class:`CollisionBox`
commands                          :class:`CommandHydro`
commands2                         :class:`CommandHydro`
contacters                        Attribute of :class:`Module`
cruisecontrol                     :class:`CruiseControl`
description                       Attribute of :class:`File`
detacher_group                    
disabledefaultsounds              Attribute of :class:`File`
enable_advanced_deformation       Attribute of :class:`File`
engine                            :class:`Engine`
engoption                         :class:`Engoption`
envmap                            
exhausts                          :class:`Exhaust`
extcamera                         :class:`ExtCamera`
fileformatversion                 Attribute of :class:`File`
fileinfo                          :class:`FileInfo`
fixes                             
flares                            :class:`Flare`
flares2                           :class:`Flare`
flexbodies                        :class:`Flexbody`
flexbody_camera_mode              :class:`CameraSettings`
flexbodywheels                    :class:`FlexBodyWheel`
forwardcommands                   Attribute of :class:`File`
fusedrag                          :class:`Fusedrag`
globals                           
guid                              Attribute of :class:`File`
guisettings                       :class:`GuiSettings`
help                              :class:`GuiSettings`
hide_in_chooser                   Attribute of :class:`File`
hookgroup                         
hooks                             :class:`Hook`
hydros                            :class:`SteeringHydro`
importcommands                    Attribute of :class:`File`
lockgroups                        :class:`Lockgroup`
lockgroup_default_nolock          
managedmaterials                  :class:`ManagedMaterial`
materialflarebindings             Attribute of :class:`Flare2`
meshwheels                        :class:`MeshWheel`
meshwheels2                       :class:`MeshWheel2`
minimass                          Attribute of :class:`File`
nodecollision                     :class:`NodeCollision`
nodes                             :class:`Node`
nodes2                            :class:`Node`
particles                         :class:`Particle`
pistonprops                       :class:`Pistonprop`
prop_camera_mode                  Attribute of :class:`Prop`
props                             :class:`Prop`
railgroups                        :class:`RailGroup`
rescuer                           Attribute of :class:`File`
rollon                            Attribute of :class:`File`
ropables                          :class:`Ropable`
ropes                             :class:`Rope`
rotators                          :class:`Rotator`
rotators2                         :class:`Rotator2`
screwprops                        :class:`Screwprop`
section                           :class:`Module`
sectionconfig                     :class:`Module`
set_beam_defaults                 :class:`BeamPreset`
set_beam_defaults_scale           :class:`BeamPreset`
set_collision_range               
set_inertia_defaults              :class:`InertiaPreset`
set_managedmaterials_options      Attributes of :class:`ManagedMaterial`
set_node_defaults                 :class:`NodePreset`
set_shadows                       
set_skeleton_settings             :class:`SkeletonSettings`
shocks                            :class:`ShockAbsorber`
shocks2                           :class:`ShockAbsorber2`
slidenode_connect_instantly       
slidenodes                        :class:`SlideNode`
slope_brake                       :class:`SlopeBrake`
soundsources                      :class:`SoundSource`
soundsources2                     :class:`SoundSource2`
speedlimiter                      :class:`SpeedLimiter`
submesh                           :class:`Submesh`
submesh_groundmodel               
texcoords                         :class:`Texcoord`
ties                              :class:`Tie`
torquecurve                       :class:`TorqueCurve`
traction_control                  :class:`TractionControl`
triggers                          :class:`Trigger`
turbojets                         :class:`Turbojet`
turboprops                        :class:`Turboprop`
turboprops2                       :class:`Turboprop`
videocamera                       :class:`VideoCamera`
wheels                            :class:`Wheel`
wheels2                           :class:`Wheel2`
wings                             :class:`Wing`
================================  ==============================
   

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
      
   .. attribute:: globals
      
   .. attribute:: nodes_by_preset
   
      List of :class:`NodeGroupWithPreset` objects.
   
   .. attribute:: beams_by_preset
   
      List of :class:`BeamGroupWithPreset` objects.
   
   .. attribute:: command_hydros_by_preset
   
      List of :class:`CommandHydroGroupWithPreset` objects.
      
   .. attribute:: shocks_by_preset
   .. attribute:: shocks2_by_preset
   .. attribute:: steering_hydros_by_preset
   .. attribute:: ropes_by_preset

   
   
   
Physics body: nodes and beams
-----------------------------

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
   
   

Physics body: special beams
---------------------------

Triggers
^^^^^^^^

.. class:: EngineTriggerFunction

   Enumeration of constants.

   .. attribute:: CLUTCH
   .. attribute:: BRAKE
   .. attribute:: ACCELERATOR
   .. attribute:: RPM_CONTROL
   .. attribute:: SHIFT_UP
   .. attribute:: SHIFT_DOWN
   .. attribute:: INVALID



.. class:: EngineTrigger

   .. attribute:: function
   .. attribute:: motor_index



.. class:: CommandKeyTrigger

   .. attribute:: contraction_trigger_key
   .. attribute:: extension_trigger_key



.. class:: HookToggleTrigger

   .. attribute:: contraction_trigger_hookgroup_id
   .. attribute:: extension_trigger_hookgroup_id



.. class:: Trigger

   .. attribute:: node_1
   .. attribute:: node_2

   .. attribute:: use_option_i_invisible
   .. attribute:: use_option_c_command_style
   .. attribute:: use_option_x_start_off
   .. attribute:: use_option_b_block_keys
   .. attribute:: use_option_b_block_triggers
   .. attribute:: use_option_a_inv_block_triggers
   .. attribute:: use_option_s_switch_cmd_num
   .. attribute:: use_option_h_unlock_hookgroups_key
   .. attribute:: use_option_h_lock_hookgroups_key
   .. attribute:: use_option_t_continuous
   .. attribute:: use_option_e_engine_trigger

   .. method:: is_hook_toggle_trigger
   .. method:: is_trigger_blocker_any_type

   .. method:: set_engine_trigger
   .. method:: get_engine_trigger

   .. method:: set_command_key_trigger
   .. method:: get_command_key_trigger

   .. method:: set_hook_toggle_trigger
   .. method:: get_hook_toggle_trigger

   .. attribute:: contraction_trigger_limit
   .. attribute:: expansion_trigger_limit
   .. attribute:: options
   .. attribute:: boundary_timer
   .. attribute:: beam_defaults
   .. attribute:: detacher_group
   .. attribute:: shortbound_trigger_action
   .. attribute:: longbound_trigger_action   


Shock absorbers
^^^^^^^^^^^^^^^

.. class:: Shock
   .. attribute:: node_1
   .. attribute:: node_2

   .. attribute:: use_option_i_invisible
   .. attribute:: use_option_l_active_left
   .. attribute:: use_option_r_active_right
   .. attribute:: use_option_m_metric

   .. attribute:: spring_rate
   .. attribute:: damping
   .. attribute:: short_bound
   .. attribute:: long_bound
   .. attribute:: precompression
   .. attribute:: options
   .. attribute:: beam_defaults
   .. attribute:: detacher_group




.. class:: Shock2   
   .. attribute:: node_1
   .. attribute:: node_2

   .. attribute:: option_i_invisible
   .. attribute:: option_s_soft_bump_bounds
   .. attribute:: option_m_metric
   .. attribute:: option_m_absolute_metric

   .. attribute:: spring_in
   .. attribute:: damp_in
   .. attribute:: progress_factor_spring_in
   .. attribute:: progress_factor_damp_in
   .. attribute:: spring_out
   .. attribute:: damp_out
   .. attribute:: progress_factor_spring_out
   .. attribute:: progress_factor_damp_out
   .. attribute:: short_bound
   .. attribute:: long_bound
   .. attribute:: precompression
   .. attribute:: beam_defaults
   .. attribute:: detacher_group


Hydraulics
^^^^^^^^^^

.. class:: SteeringHydro

   .. attribute:: node_1
   .. attribute:: node_2

   Option constants (for adding)
   
   .. attribute:: OPTION_n_NORMAL
   .. attribute:: OPTION_i_INVISIBLE
   .. attribute:: OPTION_s_DISABLE_ON_HIGH_SPEED
   .. attribute:: OPTION_a_INPUT_AILERON
   .. attribute:: OPTION_r_INPUT_RUDDER
   .. attribute:: OPTION_e_INPUT_ELEVATOR
   .. attribute:: OPTION_u_INPUT_AILERON_ELEVATOR
   .. attribute:: OPTION_v_INPUT_InvAILERON_ELEVATOR
   .. attribute:: OPTION_x_INPUT_AILERON_RUDDER
   .. attribute:: OPTION_y_INPUT_InvAILERON_RUDDER
   .. attribute:: OPTION_g_INPUT_ELEVATOR_RUDDER
   .. attribute:: OPTION_h_INPUT_InvELEVATOR_RUDDER

   Option read-only properties
   
   .. attribute:: has_option_i_invisible
   .. attribute:: has_option_s_disable_on_high_speed
   .. attribute:: has_option_a_input_aileron
   .. attribute:: has_option_r_input_rudder
   .. attribute:: has_option_e_input_elevator
   .. attribute:: has_option_u_input_aileron_elevator
   .. attribute:: has_option_v_input_invaileron_elevator
   .. attribute:: has_option_x_input_aileron_rudder
   .. attribute:: has_option_y_input_invaileron_rudder
   .. attribute:: has_option_g_input_elevator_rudder
   .. attribute:: has_option_h_input_invelevator_rudder

   Option setter
   
   .. method:: add_option

   Attributes
   
   .. attribute:: lenghtening_factor
   .. attribute:: detacher_group
   .. attribute:: inertia
   .. attribute:: inertia_defaults
   .. attribute:: beam_defaults


.. class:: CommandHydro

   Auxiliary hydraulics, commanded by keys.
   
   Represents both "commands" and "commands2" in truckfile.

   .. attribute:: node_1
   .. attribute:: node_2

   .. attribute:: option_i_invisible
   .. attribute:: option_r_rope
   .. attribute:: option_c_auto_center
   .. attribute:: option_f_not_faster
   .. attribute:: option_p_press_once
   .. attribute:: option_o_press_once_center

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


Physics body: behavior
----------------------



.. class:: CollisionBox

   .. attribute:: nodes



.. class:: Hook   

   .. attribute:: node
   .. attribute:: option_hook_range
   .. attribute:: option_speed_coef
   .. attribute:: option_max_force
   .. attribute:: option_hookgroup
   .. attribute:: option_lockgroup
   .. attribute:: option_timer
   .. attribute:: option_minimum_range_meters

   .. attribute:: use_option_self_lock
   .. attribute:: use_option_auto_lock
   .. attribute:: use_option_no_disable
   .. attribute:: use_option_no_rope
   .. attribute:: use_option_visible



.. class:: Rotator

   .. attribute:: node_1
   .. attribute:: node_2

   .. method:: get_base_plate_node
   .. method:: set_base_plate_node
   .. method:: get_rot_plate_node
   .. method:: set_rot_plate_node

   .. attribute:: rate
   .. attribute:: spin_left_key
   .. attribute:: spin_right_key
   .. attribute:: inertia
   .. attribute:: inertia_defaults
   .. attribute:: engine_coupling
   .. attribute:: needs_engine



.. class:: Rotator2

   .. attribute:: node_1
   .. attribute:: node_2

   .. method:: get_base_plate_node
   .. method:: set_base_plate_node
   .. method:: get_rot_plate_node
   .. method:: set_rot_plate_node

   .. attribute:: rate
   .. attribute:: spin_left_key
   .. attribute:: spin_right_key
   .. attribute:: inertia
   .. attribute:: inertia_defaults
   .. attribute:: engine_coupling
   .. attribute:: needs_engine
   .. attribute:: rotating_force
   .. attribute:: tolerance
   .. attribute:: description



.. class:: Lockgroup

   Constants:
   
   .. attribute:: SPECIAL_NUMBER_DEFAULT
   .. attribute:: SPECIAL_NUMBER_NOLOCK
   
   Attributes:
   
   .. attribute:: number
   .. attribute:: nodes



.. class:: NodeCollision

   .. attribute:: node
   .. attribute:: radius



.. class:: RailGroup

   .. attribute:: id
   .. attribute:: node_list



.. class:: Ropable

   .. attribute:: node
   .. attribute:: intgroup
   .. attribute:: _has_group_set
   .. attribute:: multilock
   .. attribute:: _has_multilock_set



.. class:: Rope

   .. attribute:: root_node
   .. attribute:: end_node
   .. attribute:: invisible
   .. attribute:: _has_invisible_set
   .. attribute:: beam_defaults
   .. attribute:: detacher_group



.. class:: SlideNode

   .. attribute:: slide_node
   .. attribute:: rail_nodes
   .. attribute:: spring_rate
   .. attribute:: break_force
   .. attribute:: tolerance
   .. attribute:: railgroup_id
   .. attribute:: _railgroup_id_set
   .. attribute:: attachment_rate
   .. attribute:: max_attachment_distance
   .. attribute:: _break_force_set

   .. attribute:: use_constraint_attach_all
   .. attribute:: use_constraint_attach_foreign
   .. attribute:: use_constraint_attach_self
   .. attribute:: use_constraint_attach_none



.. class:: CabTriangle

   .. method:: get_node
   .. method:: set_node
   .. method:: get_option_D_contact_buoyant
   .. method:: get_option_F_10x_tougher_buoyant
   .. method:: get_option_S_unpenetrable_buoyant

   .. attribute:: use_option_c_contact
   .. attribute:: use_option_b_buoyant
   .. attribute:: use_option_p_10xtougher
   .. attribute:: use_option_u_invulnerable
   .. attribute:: use_option_s_buoyant_no_drag
   .. attribute:: use_option_r_buoyant_only_drag



.. class:: Texcoord

   .. attribute:: u
   .. attribute:: v



.. class:: Submesh

   .. attribute:: use_backmesh
   
      Boolean. If True, the triangles' backsides will be black instead of see-through. 
   
   .. attribute:: texcoords
   
      List of :class:`Texcoord`
   
   .. attribute:: cab_triangles
   
      List of :class:`CabTriangle`



.. class:: Tie

   .. attribute:: is_invisible
   .. attribute:: root_node
   .. attribute:: max_reach_length
   .. attribute:: auto_shorten_rate
   .. attribute:: min_length
   .. attribute:: max_length
   .. attribute:: max_stress
   .. attribute:: beam_defaults
   .. attribute:: detacher_group
   .. attribute:: group
   .. attribute:: _group_set



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

 
Marine
------


.. class:: Screwprop

   .. attribute:: prop_node
   .. attribute:: back_node
   .. attribute:: top_node
   .. attribute:: power


   
Powertrain
----------


.. class:: SpeedLimiter

   .. attribute:: max_speed
   
   

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


Look and feel
-------------

.. class:: CameraSettingsMode

   Constants.

   .. attribute:: ALWAYS
   .. attribute:: EXTERNAL
   .. attribute:: CINECAM
   .. attribute:: INVALID


.. class:: CameraSettings

   .. attribute:: mode
   .. attribute:: cinecam_index


.. class:: MapMode

   Constants.

   .. attribute:: OFF
   .. attribute:: SIMPLE
   .. attribute:: ZOOM
   .. attribute:: INVALID


.. class:: GuiSettings

   .. attribute:: tacho_material
   .. attribute:: speedo_material
   .. attribute:: speedo_highest_kph
   .. attribute:: use_max_rpm
   .. attribute:: help_material
   .. attribute:: interactive_overview_map_mode
   .. attribute:: dashboard_layouts
   .. attribute:: rtt_dashboard_layouts

   
.. class:: AnimationSourceMotorDetail

   Enumeration of constants.

   .. attribute:: AERO_INVALID
   .. attribute:: AERO_THROTTLE
   .. attribute:: AERO_RPM
   .. attribute:: AERO_TORQUE
   .. attribute:: AERO_PITCH
   .. attribute:: AERO_STATUS




.. class:: AnimationMotorSource

   .. attribute:: source)
   .. attribute:: motor


.. class:: Animation

   .. attribute:: use_source_air_speed
   .. attribute:: use_source_vertical_velocity
   .. attribute:: use_source_altimeter_100k
   .. attribute:: use_source_altimeter_10k
   .. attribute:: use_source_altimeter_1k
   .. attribute:: use_source_aoa
   .. attribute:: use_source_flap
   .. attribute:: use_source_air_brake
   .. attribute:: use_source_roll
   .. attribute:: use_source_pitch
   .. attribute:: use_source_brakes
   .. attribute:: use_source_accel
   .. attribute:: use_source_clutch
   .. attribute:: use_source_speedo
   .. attribute:: use_source_tacho
   .. attribute:: use_source_turbo
   .. attribute:: use_source_parking_brake
   .. attribute:: use_source_manushift_left_right
   .. attribute:: use_source_manushift_back_forth
   .. attribute:: use_source_seqential_shift
   .. attribute:: use_source_shifterlin
   .. attribute:: use_source_torque
   .. attribute:: use_source_heading
   .. attribute:: use_source_diff_lock
   .. attribute:: use_source_boat_rudder
   .. attribute:: use_source_boat_throttle
   .. attribute:: use_source_steering_wheel
   .. attribute:: use_source_aileron
   .. attribute:: use_source_elevator
   .. attribute:: use_source_aerial_rudder
   .. attribute:: use_source_permanent
   .. attribute:: use_source_event

   .. attribute:: use_mode_rotation_x
   .. attribute:: use_mode_rotation_y
   .. attribute:: use_mode_rotation_z
   .. attribute:: use_mode_offset_x
   .. attribute:: use_mode_offset_y
   .. attribute:: use_mode_offset_z
   .. attribute:: use_mode_auto_animate
   .. attribute:: use_mode_no_flip
   .. attribute:: use_mode_bounce
   .. attribute:: use_mode_event_lock

   .. attribute:: ratio
   .. attribute:: lower_limit
   .. attribute:: upper_limit
   .. attribute:: source
   .. attribute:: motor_sources
   .. attribute:: mode

.. class:: Animator

   .. attribute:: lenghtening_factor
   .. attribute:: flags
   .. attribute:: short_limit
   .. attribute:: long_limit
   .. attribute:: aero_animator
   .. attribute:: inertia_defaults
   .. attribute:: beam_defaults
   .. attribute:: detacher_group

   .. attribute:: node_1
   .. attribute:: node_2

   .. attribute:: use_option_visible
   .. attribute:: use_option_invisible
   .. attribute:: use_option_airspeed
   .. attribute:: use_option_vertical_velocity
   .. attribute:: use_option_altimeter_100k
   .. attribute:: use_option_altimeter_10k
   .. attribute:: use_option_altimeter_1k
   .. attribute:: use_option_angle_of_attack
   .. attribute:: use_option_flap
   .. attribute:: use_option_air_brake
   .. attribute:: use_option_roll
   .. attribute:: use_option_pitch
   .. attribute:: use_option_brakes
   .. attribute:: use_option_accel
   .. attribute:: use_option_clutch
   .. attribute:: use_option_speedo
   .. attribute:: use_option_tacho
   .. attribute:: use_option_turbo
   .. attribute:: use_option_parking
   .. attribute:: use_option_shift_left_right
   .. attribute:: use_option_shift_back_forth
   .. attribute:: use_option_sequential_shift
   .. attribute:: use_option_gear_select
   .. attribute:: use_option_torque
   .. attribute:: use_option_difflock
   .. attribute:: use_option_boat_rudder
   .. attribute:: use_option_boat_throttle
   .. attribute:: use_option_short_limit
   .. attribute:: use_option_long_limit


.. class:: ManagedMaterialType

   Enumeration of constants

   .. attribute:: FLEXMESH_STANDARD
   .. attribute:: FLEXMESH_TRANSPARENT
   .. attribute:: MESH_STANDARD
   .. attribute:: MESH_TRANSPARENT
   .. attribute:: INVALID




.. class:: ManagedMaterial

   .. attribute:: type
   .. attribute:: diffuse_map
   .. attribute:: damaged_diffuse_map
   .. attribute:: specular_map
   .. attribute:: use_option_doublesided
   .. method:: has_damaged_diffuse_map
   .. method:: has_specular_map
   
   
.. class:: SpecialPropType

   .. attribute:: LEFT_REAR_VIEW_MIRROR
   .. attribute:: RIGHT_REAR_VIEW_MIRROR
   .. attribute:: STEERING_WHEEL_LEFT_HANDED
   .. attribute:: STEERING_WHEEL_RIGHT_HANDED
   .. attribute:: SPINPROP
   .. attribute:: PALE
   .. attribute:: DRIVER_SEAT
   .. attribute:: DRIVER_SEAT_2
   .. attribute:: BEACON
   .. attribute:: REDBEACON
   .. attribute:: LIGHTBAR
   .. attribute:: INVALID




.. class:: SteeringWheelSpecialProp

   .. attribute:: offset
   .. attribute:: _offset_is_set
   .. attribute:: rotation_angle
   .. attribute:: mesh_name




.. class:: BeaconSpecialProp

   .. attribute:: flare_material_name
   .. attribute:: color


.. class:: Prop

   .. attribute:: reference_node
   .. attribute:: x_axis_node
   .. attribute:: y_axis_node
   .. attribute:: Ogre::Vector3offset
   .. attribute:: Ogre::Vector3rotation
   .. attribute:: mesh_name
   .. attribute:: animations
   .. attribute:: camera_settings
   .. attribute:: special
   .. attribute:: special_prop_beacon
   .. attribute:: special_prop_steering_wheel   
   
   
.. class:: AeroAnimator

   .. attribute:: motor

   .. attribute:: use_option_throttle
   .. attribute:: use_option_rpm
   .. attribute:: use_option_torque
   .. attribute:: use_option_pitch
   .. attribute:: use_option_status

.. class:: Camera   

   .. attribute:: center_node
   .. attribute:: back_node
   .. attribute:: left_node
   
   
.. class:: CameraRail

   .. attribute:: nodes
   
.. class:: Cinecam

   .. attribute:: position
   .. attribute:: spring
   .. attribute:: damping
   .. attribute:: beam_defaults
   .. attribute:: node_defaults

   .. method:: get_node
   .. method:: set_node



.. class:: Exhaust

   .. attribute:: reference_node
   .. attribute:: direction_node
   .. attribute:: material_name


.. class:: ExtCameraMode

   .. attribute:: MODE_CLASSIC
   .. attribute:: MODE_CINECAM
   .. attribute:: MODE_NODE
   .. attribute:: MODE_INVALID



.. class:: ExtCamera

   .. attribute:: mode
   .. attribute:: node
   
   
.. class:: FlareType

   Enumeration of constants.

   .. attribute:: f_HEADLIGHT
   .. attribute:: b_BRAKELIGHT
   .. attribute:: l_LEFT_BLINKER
   .. attribute:: r_RIGHT_BLINKER
   .. attribute:: R_REVERSE_LIGHT
   .. attribute:: u_USER
   .. attribute:: INVALID




.. class:: Flare

   Represents both "flares" and "flares2" in truckfile.

   .. attribute:: reference_node
   .. attribute:: node_axis_x
   .. attribute:: node_axis_y
   .. attribute:: offset
   
      Vector3
   
   .. attribute:: type
   .. attribute:: control_number
   .. attribute:: blink_delay_milis
   .. attribute:: size
   .. attribute:: material_name
   .. attribute:: material_bindings




.. class:: Flexbody 

   .. attribute:: reference_node
   .. attribute:: x_axis_node
   .. attribute:: y_axis_node
   .. attribute:: offset
   
      Vector3
   
   .. attribute:: rotation
   
      Vector3
   
   .. attribute:: mesh_name
   .. attribute:: animations
   
      List of :class:`Animation`
   
   .. attribute:: node_list
   .. attribute:: camera_settings
   
   
   
.. class:: SkeletonSettings

   .. attribute:: visibility_range_meters
   .. attribute:: beam_thickness_meters  
   
   
.. class:: SoundSourceMode

   Enumeration of constants.

   .. attribute:: ALWAYS
   .. attribute:: OUTSIDE
   .. attribute:: CINECAM
   .. attribute:: INVALID




.. class:: SoundSource

   .. attribute:: name
   .. attribute:: sound_script_name




.. class:: SoundSource2

   .. attribute:: name
   .. attribute:: sound_script_name
   .. attribute:: mode
   
      :class:`SoundSourceMode`
   
   .. attribute:: cinecam_index


.. class:: VideoCamera

   .. attribute:: offset
   .. attribute:: rotation
   .. attribute:: left_node
   .. attribute:: camera_name
   .. attribute:: camera_role
   .. attribute:: camera_mode
   .. attribute:: bottom_node
   .. attribute:: field_of_view
   .. attribute:: texture_width
   .. attribute:: material_name
   .. attribute:: texture_height
   .. attribute:: reference_node
   .. attribute:: min_clip_distance
   .. attribute:: max_clip_distance
   .. attribute:: alt_reference_node
   .. attribute:: alt_orientation_node



.. class:: Particle

   .. attribute:: emitter_node
   .. attribute:: reference_node
   .. attribute:: particle_system_name
               


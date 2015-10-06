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
      

Data structures: Containers
---------------------------      

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
   
   
   
Data structures: Physics body
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
   

Project status
--------------

   List of all available truckfile keywords with comments on availability
   through Python.

   * KEYWORD_ADD_ANIMATION
   * KEYWORD_AIRBRAKES
   * KEYWORD_ANIMATORS
   * KEYWORD_ANTI_LOCK_BRAKES
   * KEYWORD_AXLES
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
   * KEYWORD_ENGINE
   * KEYWORD_ENGOPTION
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
   * KEYWORD_FLEXBODYWHEELS
   * KEYWORD_FORWARDCOMMANDS
   * KEYWORD_FUSEDRAG
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
   * KEYWORD_MESHWHEELS
   * KEYWORD_MESHWHEELS2
   * KEYWORD_MINIMASS
   * KEYWORD_NODECOLLISION
   * KEYWORD_NODES
   * KEYWORD_NODES2
   * KEYWORD_PARTICLES
   * KEYWORD_PISTONPROPS
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
   * KEYWORD_SET_INERTIA_DEFAULTS
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
   * KEYWORD_TORQUECURVE
   * KEYWORD_TRACTION_CONTROL
   * KEYWORD_TRIGGERS
   * KEYWORD_TURBOJETS
   * KEYWORD_TURBOPROPS
   * KEYWORD_TURBOPROPS2
   * KEYWORD_VIDEOCAMERA
   * KEYWORD_WHEELS
   * KEYWORD_WHEELS2
   * KEYWORD_WINGS


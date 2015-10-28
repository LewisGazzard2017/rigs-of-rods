:mod:`rig` --- RigEditor's internal representation of the Rig
=============================================================

.. module:: rig
   :synopsis: RigEditor's internal representation of the Rig

.. moduleauthor:: Petr Ohlidal <_myname_@_mysurname_.cz>


Nodes
-----

.. autoclass:: NodeBuffer
   :members:

.. autoclass:: Node
   :members:
        

Beams        
-----
        
.. class:: BeamType

   .. attribute:: PLAIN

   .. attribute:: ROPE

      Section 'beams' + option 'r' in truck format

   .. attribute:: SUPPORT

      Section 'beams' + option 's' in truck format

   .. attribute:: STEERING_HYDRO
   
      Section 'hydros' in truckfile

   .. attribute:: COMMAND_HYDRO
      
      Sections 'commands' or 'commands2' in truckfile

   .. attribute:: SHOCK_ABSORBER

      Section 'shocks' in truckfile

   .. attribute:: SHOCK_ABSORBER_2

      Section 'shocks2' in truckfile

   .. attribute:: CINECAM

      Section 'cinecam' in truckfile - generated beams.



.. autoclass:: BeamBuffer
   :members:
   
.. autoclass:: BeamBase
   :members:
        


.. autoclass:: PlainBeam(BeamBase)
   :members:

.. autoclass:: RopeBeam(BeamBase)
   :members:

.. autoclass:: SupportBeam(BeamBase)
   :members:

.. autoclass:: SteeringHydro(BeamBase)
   :members:

.. autoclass:: CommandHydro(BeamBase)
   :members:        
         
.. autoclass:: ShockAbsorber(BeamBase)
   :members:        
   
.. autoclass:: ShockAbsorber2(BeamBase)
   :members:        
   
   
Powertrain
----------


.. autoclass:: InertiaPreset
   :members:
   

Wheels
------

.. class:: WheelBraking:
	NO                = "no"
	YES               = "yes"
	DIRECTIONAL_LEFT  = "directional_left"
	DIRECTIONAL_RIGHT = "directional_right"
	ONLY_FOOT         = "only_foot"
	INVALID           = "INVALID_SETTING"
    
    
    
.. class:: WheelPropulsion:
	NONE     = "none"
	FORWARD  = "forward"
	BACKWARD = "backward"
	INVALID  = "INVALID_SETTING"



.. autoclass:: WheelBase
   :members:



.. autoclass:: Wheel(WheelBase)
   :members:



.. autoclass:: WheelBase2(WheelBase)
   :members:	                



.. autoclass:: Wheel2(WheelBase2)
   :members:



.. autoclass:: MeshWheel(WheelBase)
   :members:



.. autoclass:: MeshWheel2(WheelBase2)
   :members:
        
        

.. autoclass:: FlexBodyWheel(WheelBase2)
   :members:
        

Flares (lights)
---------------

.. class:: FlareType:

   .. attribute:: headlight
   
      Constant: "type_f_headlight"
      
   .. attribute:: brakelight
   
      Constant: "type_b_brakelight"
      
   .. attribute:: left_blinker
   
      Constant: "type_l_left_blinker"
      
   .. attribute:: right_blinker
   
      Constant: "type_r_right_blinker"
      
   .. attribute:: reverse_light
   
      Constant: "type_R_reverse"
      
   .. attribute:: user
   
      Constant: "type_u_user"
      
   .. attribute:: invalid
   
      Constant: "type_INVALID"
        
        
        
.. autoclass:: Flare
   :members:
            

The Rig
------- 

.. autoclass:: Rig
   :members:
                
        
        

   


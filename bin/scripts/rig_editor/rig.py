
# Module "rig" FOOOO
# Physics structure classes

from datatypes import Color
from euclid3 import Vector3, Vector2
import util
import ror_drawing

# ------------------------------------------------------------------------------
# Nodes

class NodeBuffer:
    ''' 
    
    .. attribute:: name
    .. attribute:: editor_color
    .. attribute:: parent_rig
    
    **Directive "set_node_defaults" in truckfile:**
    
    .. attribute:: default_weight
    .. attribute:: friction
    .. attribute:: volume
    .. attribute:: surface
    
    **Options:**
    
    .. attribute:: option_n_mouse_grab       
    .. attribute:: option_m_no_mouse_grab    
    .. attribute:: option_f_no_sparks        
    .. attribute:: option_c_no_ground_contact
    .. attribute:: option_h_hook_point       
    .. attribute:: option_b_extra_buoyancy   
    .. attribute:: option_p_no_particles     
    
    '''

    def __init__(self, name, parent_rig):
        self.name = name
        self.editor_color = Color.from_rgb(1,1,1)
        self.parent_rig = parent_rig
        
        # "set_node_defaults"
        self.default_weight = 0 # can be overriden per-node
        self.friction = 0
        self.volume = 0
        self.surface = 0
        
        # Options
        self.option_n_mouse_grab = False
        self.option_m_no_mouse_grab = False
        self.option_f_no_sparks = False
        self.option_c_no_ground_contact = False
        self.option_h_hook_point = False
        self.option_b_extra_buoyancy = False
        self.option_p_no_particles = False
        
    def create_node(self, name, position):
        '''
        :param str name: Unique node name
        :param euclid3.Vector3 position: Position
        :returns: :class:`Node`
        '''
        n = Node(name, self)
        self.parent_rig._nodes[name] = n
        n.position = position
        n.curr_pos = position
        return n
        
        

class Node:
    ''' 
    
    .. attribute:: name
    .. attribute:: node_buffer
    .. attribute:: position
    .. attribute:: is_hovered
    .. attribute:: is_selected
    .. attribute:: curr_pos
    
       Vector3; Represents intermediate position during transformations.
    
    **Directive "set_node_defaults" in truckfile:**
    
    .. attribute:: default_weight
    .. attribute:: friction
    .. attribute:: volume
    .. attribute:: surface
    
    **Options (None to inherit from NodeBuffer, True to override):**
    
    Note: truckfile flags x/y (exhaust) are auto-converted to section "exhaust".
    
    .. attribute:: option_n_mouse_grab       
    .. attribute:: option_m_no_mouse_grab    
    .. attribute:: option_f_no_sparks        
    .. attribute:: option_c_no_ground_contact
    .. attribute:: option_h_hook_point       
    .. attribute:: option_b_extra_buoyancy   
    .. attribute:: option_p_no_particles     
    
    '''

    def __init__(self, name, node_buffer):
        self.name = name
        self.node_buffer = node_buffer
        self.position = Vector3(0,0,0)
        self.is_hovered = False
        self.is_selected = False
        self.curr_pos = Vector3(0,0,0)
            # Current position;
            # represents intermediate position during transformations.
        self.curr_screen_pos = Vector2(0,0)
        self.curr_screen_color = Color.from_rgb(1,1,1)                                      
        self.linked_beams = []
        self.weight = None # None = Inherit from nodebuffer
        self.detacher_group = None
        
        # Options. None = inherit from nodebuffer
        # Note: flags x/y (exhaust) are auto-converted to section "exhaust"
        self.option_n_mouse_grab        = None
        self.option_m_no_mouse_grab     = None
        self.option_f_no_sparks         = None
        self.option_c_no_ground_contact = None
        self.option_h_hook_point        = None
        self.option_b_extra_buoyancy    = None
        self.option_p_no_particles      = None
        
# ------------------------------------------------------------------------------
# Beams        
        
class BeamType:
    PLAIN            = "plain" 
    ROPE             = "rope" # Option 'r' in truck format
    SUPPORT          = "support" # Option 's' in truck format
    STEERING_HYDRO   = "steering_hydro" # Called 'hydro' in RoR jargon
    COMMAND_HYDRO    = "command_hydro" # Called 'command' in RoR jargon
    SHOCK_ABSORBER   = "shock_absorber" # Called 'shock' in RoR jargon
    SHOCK_ABSORBER_2 = "shock_absorber_2" # Section 'shocks2'
    CINECAM          = "cinecam" # Section 'cinecam' - generated



class BeamBuffer:

    def __init__(self, parent_rig, name, beam_type=BeamType.PLAIN):
        self.parent_rig = parent_rig
        self.name = name
        self.beam_type = beam_type
        # Directive 'set_beam_defaults' from .truck format
        self.spring = 0
        self.damping = 0
        self.deformation_threshold = 0
        self.breaking_threshold = 0
        self.visual_thickness = 0
        self.visual_material_name = None
        self.plastic_deform_coef = 0
        self.enable_advanced_deformation = True
        self.editor_color = Color.from_rgb(1,1,1)

    def create_beam(self, node1, node2):
        if   self.beam_type == BeamType.ROPE:
            beam = RopeBeam(self, node1, node2)
        elif self.beam_type == BeamType.SUPPORT:
            beam = SupportBeam(self, node1, node2)
        elif self.beam_type == BeamType.STEERING_HYDRO:
            beam = SteeringHydro(self, node1, node2)
        elif self.beam_type == BeamType.COMMAND_HYDRO:
            beam = CommandHydro(self, node1, node2)
        elif self.beam_type == BeamType.SHOCK_ABSORBER:
            beam = ShockAbsorber(self, node1, node2)
        elif self.beam_type == BeamType.SHOCK_ABSORBER_2:
            beam = ShockAbsorber2(self, node1, node2)
        else:
            beam = PlainBeam(self, node1, node2)

        self.parent_rig._beams.append(beam)
        return beam


class BeamBase:
    
    def __init__(self, beam_buffer, node1, node2):
        self.beam_buffer = beam_buffer
        self.node1 = node1
        self.node2 = node2
        self.curr_color = Color.from_rgb(1,1,1)
        self.detacher_group = 0
        


class PlainBeam(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)



class RopeBeam(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)



class SupportBeam(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)
        self.extension_break_limit = 0 # 0 = Default in .truck format



class SteeringHydro(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)
        self.disable_at_high_speed            = False # Option 's' in .truck
        self.option_input_aileron             = False
        self.option_input_aileron_inverted    = False
        self.option_input_rudder              = False
        self.option_input_elevator            = False
        self.option_input_elevator_inverted   = False
        self.inertia = None
        


class CommandHydro(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)
        self.version = 2
        self.shorten_rate = 0
        self.lengthen_rate = 0
        self.max_contraction = 0
        self.max_extension = 0
        self.contract_key = 0
        self.extend_key = 0
        self.inertia = None
        self.description = None
        self.affect_engine = 0
        self.needs_engine = False
        self.option_i_invisible         = False
        self.option_r_rope              = False
        self.option_c_auto_center       = False
        self.option_f_not_faster        = False
        self.option_p_press_once        = False
        self.option_o_press_once_center = False

        
         
class ShockAbsorber(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)
        self.max_contraction = 0 # proportion of length. "0" means the shock will not be able to contract at all, "1" will let it contract all the way to zero length. If the shock tries to shorten more than this value allows, it will become as rigid as a normal beam. 
        self.max_extension = 0 # a proportion of length. "0" means the shock will not be able to extend at all. "1" means the shock will be able to double its length. Higher values allow for longer extension.
        self.precompression = 1 # On-spawn value (proportion of length). This can be used to "level" the suspension of a truck if it sags in game. The default value is 1.0.
        self.active_left = False  # Option 'L' in .truck format
        self.active_right = False # Option 'R' in .truck format



class ShockAbsorber2(BeamBase):

    def __init__(self, buffer, node1, node2):
        BeamBase.__init__(self, buffer, node1, node2)
        self.max_contraction = 0 # proportion of length. "0" means the shock will not be able to contract at all, "1" will let it contract all the way to zero length. If the shock tries to shorten more than this value allows, it will become as rigid as a normal beam. 
        self.max_extension = 0 # a proportion of length. "0" means the shock will not be able to extend at all. "1" means the shock will be able to double its length. Higher values allow for longer extension.
        self.precompression = 1 # On-spawn value (proportion of length). This can be used to "level" the suspension of a truck if it sags in game. The default value is 1.0.
        self.use_soft_bump_bounds = False
        self.spring_in = 0                  # Spring value applied when the shock is compressing.
        self.damp_in = 0                    # Damping value applied when the shock is compressing. 
        self.progress_factor_spring_in = 0  # Progression factor for springin. A value of 0 disables this option. 1...x as multipliers, example:maximum springrate == springrate + (factor*springrate)
        self.progress_factor_damp_in = 0    # Progression factor for dampin. 0 = disabled, 1...x as multipliers, example:maximum dampingrate == springrate + (factor*dampingrate)
        self.spring_out = 0                 # spring value applied when shock extending
        self.damp_out = 0                   # damping value applied when shock extending
        self.progress_factor_spring_out = 0 # Progression factor springout, 0 = disabled, 1...x as multipliers, example:maximum springrate == springrate + (factor*springrate)
        self.progress_factor_damp_out = 0   # Progression factor dampout, 0 = disabled, 1...x as multipliers, example:maximum dampingrate == springrate + (factor*dampingrate) 

# ------------------------------------------------------------------------------
# Inertia

class InertiaPreset:
    
    def __init__(self, name):
        self.name = name
        self.start_delay_factor = None
        self.stop_delay_factor = None
        self.start_function = None
        self.stop_function = None


# ------------------------------------------------------------------------------
# Wheels

class WheelBraking:
	NO                = "no"
	YES               = "yes"
	DIRECTIONAL_LEFT  = "directional_left"
	DIRECTIONAL_RIGHT = "directional_right"
	ONLY_FOOT         = "only_foot"
	INVALID           = "INVALID_SETTING"
    
    
    
class WheelPropulsion:
	NONE     = "none"
	FORWARD  = "forward"
	BACKWARD = "backward"
	INVALID  = "INVALID_SETTING"



class WheelBase:
    
    def __init__(self, node1, node2):
        self.name = "Wheel_" + str(id(self))
        self.node1 = node1
        self.node2 = node2
        self.num_rays = 0
        self.rigidity_node = None
        self.braking = WheelBraking.INVALID
        self.propulsion = WheelPropulsion.INVALID
        self.reference_arm_node = None
        self.mass = 0
        self.node_buffer = None
        self.beam_buffer = None



class Wheel(WheelBase):

    def __init__(self, node1, node2):
        WheelBase.__init__(self, node1, node2)
        self.radius = 0
        self.springiness = 0
        self.damping = 0
        self.face_material_name = "tracks/wheelface"
        self.band_material_name = "tracks/wheelband1"



class WheelBase2(WheelBase):

    def __init__(self, node1, node2):
        WheelBase.__init__(self, node1, node2)
        self.rim_radius       = 0
        self.tyre_radius      = 0
        self.tyre_springiness = 0
        self.tyre_damping     = 0	                



class Wheel2(WheelBase2):

    def __init__(self, node1, node2):
        WheelBase2.__init__(self, node1, node2)
        self.face_material_name = "tracks/wheelface"
        self.band_material_name = "tracks/wheelband1"
        self.rim_springiness = 0
        self.rim_damping = 0



class MeshWheel(WheelBase):

    def __init__(self, node1, node2):
        WheelBase.__init__(self, node1, node2)
        self.side = 0
        self.mesh_name = None
        self.material_name = None
        self.rim_radius = 0
        self.tyre_radius = 0
        self.spring = 0
        self.damping = 0



class MeshWheel2(WheelBase2):

    def __init__(self, node1, node2):
        WheelBase2.__init__(self, node1, node2)
        # Rim is set-up by `beam_defaults`, params in section are for tire.
        self.side = 0
        self.mesh_name = None
        self.material_name = None
        
        

class FlexBodyWheel(WheelBase2):

    def __init__(self, node1, node2):
        WheelBase2.__init__(self, node1, node2)
        self.side = 0
        self.rim_springiness = 0
        self.rim_damping = 0
        self.rim_mesh_name = None
        self.tyre_mesh_name = None
        
# ------------------------------------------------------------------------------
# Flares (lights)

class FlareType:
    headlight     = "type_f_headlight"
    brakelight    = "type_b_brakelight"
    left_blinker  = "type_l_left_blinker"
    right_blinker = "type_r_right_blinker"
    reverse_light = "type_R_reverse"
    user          = "type_u_user"
    invalid       = "type_INVALID"
        
        
        
class Flare:

    def __init__(self, parent_rig, origin_node, x_axis_node, y_axis_node):
        self.parent_rig = parent_rig
        self.name = str(id(self))
        self.flare_type = FlareType.invalid
        self.node_origin = origin_node
        self.node_axis_x = x_axis_node
        self.node_axis_y = y_axis_node
        self.offset = Vector3(0,0,0)
        self.control_number = 0
        self.blink_delay_milis = 0
        self.size = 0
        self.material_name = None
        
    def rename(self, new_name):
        self.parent_rig.remove_flare(self)
        self.name = new_name
        self.parent_rig.insert_flare(self)
            

# ------------------------------------------------------------------------------
# The big Rig 

class Rig:
    '''
    .. attribute:: name 
    '''

    def __init__(self, config):
        # Meta
        self.name = None
        # Structure
        self._nodes = {}
        self._node_buffers = {}
        self._beams = []
        self._beam_buffers = {}
        self._inertia_presets = {}
        self._wheels = []
        self._flares = {}
        
        # Visuals
        self._config = config
        node_conf = config["nodes_display"]
        
        idle_point_size = node_conf["node_idle_point_size"]["value"]
        self.idle_nodes_mesh = ror_drawing.create_points_mesh(idle_point_size)
        self.idle_nodes_mesh.attach_to_scene()
        
        hover_point_size = node_conf["node_hovered_point_size"]["value"]
        self.hovered_nodes_mesh = ror_drawing.create_points_mesh(hover_point_size)
        self.hovered_nodes_mesh.attach_to_scene()
        
        select_point_size = node_conf["node_selected_point_size"]["value"]
        self.selected_nodes_mesh = ror_drawing.create_points_mesh(select_point_size)
        self.selected_nodes_mesh.attach_to_scene()
        
        self.beams_mesh = ror_drawing.create_lines_mesh()
        self.beams_mesh.attach_to_scene()
        
    def create_node_buffer(self, name=None):
        '''
        :param str|None name: If none, a random ID {NodeBuffer_######} will be generated.
        '''
        if name is None: 
            name = util.generate_random_unique_id(self._node_buffers, "NodeBuffer_")
        if name in self._node_buffers:
            raise Exception("Duplicate name of 'NodeBuffer': ", name)
        buff = NodeBuffer(name, self)
        self._node_buffers[name] = buff
        return buff
        
    def create_beam_buffer(self, name=None, beam_type=BeamType.PLAIN):
        '''
        :param str|None name: If none, a random ID {BeamBuffer_######} will be generated.
        '''
        if name is None:
            name = util.generate_random_unique_id(self._beam_buffers, "BeamBuffer_")
        if name in self._beam_buffers:
            raise Exception("Duplicate name of 'BeamBuffer': ", name)
        buff = BeamBuffer(self, name, beam_type)
        self._beam_buffers[name] = buff
        return buff             
        
    def create_inertia_preset(self, name=None):
        '''
        :param str|None name: If none, a random ID {InertiaPreset_######} will be generated.
        '''
        if name is None:
            name = util.generate_random_unique_id(self._inertia_presets, "InertiaPreset_")
        if name in self._inertia_presets:
            raise Exception("Duplicate name of 'InertiaPreset': ", name)
        inertia = InertiaPreset(name)
        self._inertia_presets[name] = inertia
        return inertia
        
    def add_wheel(self, wheel):
        ''' '''
        self._wheels.append(wheel)
        
    def create_flare(self, origin_node, x_node, y_node, name=None):
        ''' '''
        flare = Flare(self, origin_node, x_node, y_node)
        if name is not None:
            flare.name = name
        self.insert_flare(flare)
        return flare
        
    def get_flare_by_name(self, name):
        ''' '''
        return self._flares[name]
        
    def remove_flare_by_name(self, name):
        ''' '''
        del self._flares[name]
        
    def remove_flare(self, flare):
        ''' '''
        self.remove_flare_by_name(flare.name)
        
    def insert_flare(self, flare):
        ''' '''
        self._flares[flare.name] = flare
        
    def loop_flare_values(self):
        ''' '''
        return self._flares.values()
        
    def loop_node_objects(self):
        ''' '''
        return self._nodes.values()
        
    def loop_beam_objects(self):
        ''' '''
        return self._beams
        
    def loop_node_buffer_objects(self):
        ''' '''
        return self._node_buffers.values()
        
    def find_node(self, name):
        ''' '''
        if name not in self._nodes:
            raise Exception("Failed to find node [" + name + "]")
        else:
            return self._nodes[name]
            
    def get_node_buffer(self, name):
        ''' '''
        return self._node_buffers[name]
                
    def colorize_nodes_default_scheme(self):                                 
        '''
        Updates color value in :class:`Node` objects. Doesn't repaint screen.
        '''
        node_conf = self._config["nodes_display"]
        node_idle_color   = node_conf["node_idle_color"]["value"]                  
         
        for node in self.loop_node_objects():
            node.curr_screen_color = node_idle_color
            
    def colorize_beams_default_scheme(self):                                 
        '''
        Updates color value in :class:`Beam` objects. Doesn't repaint screen.
        '''
        beam_conf = self._config["beams_display"]
        
        colors = {
            "plain":   beam_conf["beam_generic_color"]["value"],
            "support": beam_conf["beam_support_color"]["value"],
            "rope":    beam_conf["beam_rope_color"]["value"],
            "support": beam_conf["beam_support_color"]["value"],
            "command": beam_conf["command_hydro_beam_color"]["value"],
            "steering":beam_conf["steering_hydro_beam_color"]["value"],
            "shock_1": beam_conf["shock_absorber_beam_color"]["value"],
            "shock_2": beam_conf["shock_absorber_2_beam_color"]["value"]
        }                  
        
        for beam in self.loop_beam_objects():
            t = type(beam)
            if   t is PlainBeam:
                beam.curr_color = colors["plain"]
            elif t is RopeBeam:
                beam.curr_color = colors["rope"]
            elif t is SupportBeam:
                beam.curr_color = colors["support"]
            elif t is ShockAbsorber:
                beam.curr_color = colors["shock_1"]
            elif t is ShockAbsorber2:
                beam.curr_color = colors["shock_2"]
            elif t is SteeringHydroBeam:
                beam.curr_color = colors["steering"]
            elif t is CommandHydroBeam:
                beam.curr_color = colors["command"]
            elif t is PlainBeam:
                beam.curr_color = colors["plain"]
         
        
    
    def update_node_meshes(self):
        '''
        Updates screen visuals. Doesn't change node object data.
        '''
        node_conf = self._config["nodes_display"]
        node_hover_color  = node_conf["node_hovered_color"]["value"]
        node_select_color = node_conf["node_selected_color"]["value"]
        
        self.idle_nodes_mesh.begin_update()
        self.hovered_nodes_mesh.begin_update()
        self.selected_nodes_mesh.begin_update()
        
        for node in self.loop_node_objects():
            if node.is_hovered is True:
                self.hovered_nodes_mesh.add_point(node.curr_pos, node_hover_color)
            elif node.is_selected is True:
                self.selected_nodes_mesh.add_point(node.curr_pos, node_select_color)
            else:
                self.idle_nodes_mesh.add_point(node.curr_pos, node.curr_screen_color)
                
        self.idle_nodes_mesh.end_update()
        self.hovered_nodes_mesh.end_update()
        self.selected_nodes_mesh.end_update()
        
    def update_beams_mesh(self):
        '''
        Updates screen visuals. Doesn't change beam object data.
        '''
        self.beams_mesh.set_position(Vector3(0, 0, 0))
        self.beams_mesh.begin_update()
        for beam in self.loop_beam_objects():
            pos1 = beam.node1.curr_pos
            pos2 = beam.node2.curr_pos
            #print("@add_line() Positions:", pos1, pos2, "color:", beam.curr_color)
            self.beams_mesh.add_line(pos1, beam.curr_color, pos2, beam.curr_color)           
            
        self.beams_mesh.end_update()
        self.beams_mesh.attach_to_scene()
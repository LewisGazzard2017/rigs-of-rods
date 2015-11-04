
# Module "truckfile"
# Truckfile loading and serialization

def load(config, directory, filename):
    import ror_truckfile
    parser = ror_truckfile.Parser()
    parser.parse_file(directory, filename)
    truck = parser.get_parsed_file()
    if (truck is None):
        return None
    
    from rig import Rig, BeamType
    rig = Rig(config)
    
    import euclid3
    
    # ******** Functions ******** #
    
    def import_vector3(v3): # Transform any Vector3 type to euclid3.Vector3
        return euclid3.Vector3(v3.x, v3.y, v3.z)
    
    def fetch_node(rig, node_ref):
        return rig.find_node(node_ref.get_id_str())
    
    def process_module(module, rig):
        # === Nodes ===
        for node_group in module.nodes_by_preset:
            # Setup buffer
            preset = node_group.preset
            buff = rig.create_node_buffer()
            
            buff.default_weight = preset.load_weight
            buff.friction       = preset.friction
            buff.volume         = preset.volume
            buff.surface        = preset.surface
            
            buff.option_n_mouse_grab        = preset.option_n
            buff.option_m_no_mouse_grab     = preset.option_m
            buff.option_f_no_sparks         = preset.option_f
            buff.option_c_no_ground_contact = preset.option_c
            buff.option_h_hook_point        = preset.option_h
            buff.option_b_extra_buoyancy    = preset.option_b
            buff.option_p_no_particles      = preset.option_p
            # Insert nodes
            for truck_node in node_group.nodes:
                name = truck_node.id.id_str
                position = import_vector3(truck_node.position)
                node = buff.create_node(name, position)
                if truck_node._has_load_weight_override:
                    node.weight = truck_node.load_weight_override
                node.detacher_group = truck_node.detacher_group
                # Flag overrides - if True, then set True, otherwise set None to inherit from buffer.
                node.option_n_mouse_grab        = True if (truck_node.option_n is True) else None
                node.option_m_no_mouse_grab     = True if (truck_node.option_m is True) else None
                node.option_f_no_sparks         = True if (truck_node.option_f is True) else None
                node.option_c_no_ground_contact = True if (truck_node.option_c is True) else None
                node.option_h_hook_point        = True if (truck_node.option_h is True) else None
                node.option_b_extra_buoyancy    = True if (truck_node.option_b is True) else None
                node.option_p_no_particles      = True if (truck_node.option_p is True) else None
                
        # === Beams ===
        def process_beam_list(rig, beam_list, beam_type):
            if len(beam_list) is 0:
                return
            buff = rig.create_beam_buffer(None, beam_type)
            for truck_beam in beam_list:
                node1 = rig.find_node(truck_beam.node_1.get_id_str())
                node2 = rig.find_node(truck_beam.node_2.get_id_str())
                beam = buff.create_beam(node1, node2)
            
        for beam_group in module.beams_by_preset:
            # Sort by types
            plain_beams   = []
            support_beams = []
            rope_beams    = []
            for truck_beam in beam_group.beams:
                if   truck_beam.option_s_support is True:
                    support_beams.append(truck_beam)
                elif truck_beam.option_r_rope is True:
                    rope_beams.append(truck_beam)
                else:
                    plain_beams.append(truck_beam)
            # Process by type
            #print("DBG beamlist plain")
            process_beam_list(rig, plain_beams,   BeamType.PLAIN)
            #print("DBG beamlist support")
            process_beam_list(rig, support_beams, BeamType.SUPPORT)
            #print("DBG beamlist rope")
            process_beam_list(rig, rope_beams,    BeamType.ROPE)
            
        # === Flares (lights) ===
        #print("DBG ---- truckfile: processing flares ----")
        for truck_flare in module.flares:
            ref_node = fetch_node(rig, truck_flare.reference_node)
            x_node   = fetch_node(rig, truck_flare.node_axis_x)
            y_node   = fetch_node(rig, truck_flare.node_axis_y)
            flare = rig.create_flare(ref_node, x_node, y_node)
            
    # ******** Processing ******** #
    
    rig.name = truck.name
    process_module(truck.root_module, rig)
        
    return rig



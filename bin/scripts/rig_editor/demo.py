
# Tests of python bindings

class Demo:

    def draw_demo_mesh():
        import ror_drawing
        from euclid3 import Vector3
        from datatypes import Color
        
        mesh = ror_drawing.create_lines_mesh();
        mesh.set_position(Vector3(0,0,0))
        mesh.begin_update()
        mesh.add_line(Vector3(0, 0, 0), Color(1, 1 ,0),     Vector3(5, 0, 0), Color(1, 0, 0))
        mesh.add_line(Vector3(0, 0, 0), Color(0.2, 0.8 ,1), Vector3(0, 0, 5), Color(0, 0, 1))
        mesh.add_line(Vector3(0, 0, 0), Color(0.1, 1, 0.7), Vector3(0, 5, 0), Color(0, 1, 0))
        mesh.end_update()
        mesh.attach_to_scene()
        
    def test_truckfile_import(directory, filename):
        import ror_truckfile
    
        parser = ror_truckfile.Parser()
        parser.parse_file(directory, filename)
        truck = parser.get_parsed_file()
        print("-- TRUCK --")
        print("name: ", truck.name)
        print("-- ROOT MODULE --")
        m = truck.root_module
        print("name:", m.name)
        m.name = "NameTest" # Does string assignment work?
        print("name test: [" + truck.root_module.name + "]")
        
        # Note: For easy viewing of the output, 
        # sections are ordered by amount of data they output (roughly)
        
        # Powertrain
        print (" -- POWERTRAIN -- ")
        
        if (m.engine is not None):
            text = "\tEngine | ";
            text += "torque: " + str(m.engine.torque)
            # Gears
            text += "\n\t\tGears (" + str(len(m.engine.gear_ratios) ) + "): "
            for ratio in m.engine.gear_ratios:
                text += str(ratio) + " "
            print(text)
        
        if (m.engoption is not None):
            eo = m.engoption
            text = "\tEngoption | "
            text += " type: " + str(eo.type)
            text += " stallRPM: " + str(eo.stall_rpm)
            text += " idleRPM: " + str(eo.idle_rpm)
            print(text)
          
        if (m.torque_curve is not None):
            tc = m.torque_curve
            text = "\tTorqueCurve | samples: " + str(len(tc.samples) ) + "; name: " + tc.predefined_func_name
            for sample in tc.samples:
                text += "\n\t\tSample | power: " + str(sample.power) + ", torque_percent:" + str(sample.torque_percent)
            print(text)
          
        print("Axles (", len(m.axles), ")")
        for axle in m.axles:
            text = "\tAxle | Nodes:"
            text += "\n\t\t" + axle.wheel1_node1.get_id_str()
            text += "\n\t\t" + axle.wheel1_node2.get_id_str()
            text += "\n\t\t" + axle.wheel2_node1.get_id_str()
            text += "\n\t\t" + axle.wheel2_node2.get_id_str()
            text += "\n\tOptions ("+str(len(axle.options))+"):"
            for opt in axle.options:
               text += " " + str(opt)
            print(text)
          
        # Aerial 
        print(" -- AERIAL -- ")
        
        print ("Wings ( count: ", len(m.wings), "):")
        for wing in m.wings:
            text = "\tWing | "
            text+="\n\t\t control_surface: "+ str(wing.control_surface)
            text+="\n\t\t chord_point: "+     str(wing.chord_point)
            text+="\n\t\t min_deflection: "+  str(wing.min_deflection)
            text+="\n\t\t max_deflection: "+  str(wing.max_deflection)
            text+="\n\t\t airfoil_name: "+    str(wing.airfoil_name)
            text+="\n\t\t efficacy_coef: "+   str(wing.efficacy_coef)
            print(text)
          
        print ("Airbrakes ( count: ", len(m.airbrakes), "):")
        for airbrake in m.airbrakes:
            text = "\tAirbrake | " 
            text+="\n\t\t reference_node: "+        airbrake.reference_node.get_id_str()
            text+="\n\t\t x_axis_node: "+           airbrake.x_axis_node.get_id_str()
            text+="\n\t\t y_axis_node: "+           airbrake.y_axis_node.get_id_str()
            text+="\n\t\t aditional_node: "+        airbrake.aditional_node.get_id_str()
            text+="\n\t\t offset: "+                str(airbrake.offset)
            text+="\n\t\t width: "+                 str(airbrake.width)
            text+="\n\t\t height: "+                str(airbrake.height)
            text+="\n\t\t max_inclination_angle: "+ str(airbrake.max_inclination_angle)
            text+="\n\t\t texcoord_x1: "+           str(airbrake.texcoord_x1)
            text+="\n\t\t texcoord_x2: "+           str(airbrake.texcoord_x2)
            text+="\n\t\t texcoord_y1: "+           str(airbrake.texcoord_y1)
            text+="\n\t\t texcoord_y2: "+           str(airbrake.texcoord_y2)
            text+="\n\t\t lift_coefficient: "+      str(airbrake.lift_coefficient)
            print(text)
          
        print ("Turbojets ( count: ", len(m.turbojets), "):")
        for item in m.turbojets:
            text = "\Turbojet | "   
            text+="\n\t\t front_node: "+     str(item.front_node.get_id_str())
            text+="\n\t\t back_node: "+      str(item.back_node.get_id_str())
            text+="\n\t\t side_node: "+      str(item.side_node.get_id_str())
            text+="\n\t\t is_reversable: "+  str(item.is_reversable)
            text+="\n\t\t dry_thrust: "+     str(item.dry_thrust)
            text+="\n\t\t wet_thrust: "+     str(item.wet_thrust)
            text+="\n\t\t front_diameter: "+ str(item.front_diameter)
            text+="\n\t\t back_diameter: "+  str(item.back_diameter)
            text+="\n\t\t nozzle_length: "+  str(item.nozzle_length)
            print(text)
        
        
        
        print ("Turboprop ( count: ", len(m.turboprops), "):")
        for item in m.turboprops:
            text = "\Turboprop | "   
            text+="\n\t\t reference_node: "+   str(item.reference_node.get_id_str())
            text+="\n\t\t axis_node: "+        str(item.axis_node.get_id_str())
            text+="\n\t\t turbine_power_kW: "+ str(item.turbine_power_kW)
            text+="\n\t\t airfoil: "+          str(item.airfoil)
            text+="\n\t\t couple_node: "+      str(item.couple_node.get_id_str())
            text+="\n\t\t _format_version: "+  str(item._format_version)
            # .def("get_blade_tip_node: "+         &Turboprop2::PY_GetBladeTipNode)
            # .def("set_blade_tip_node: "+         &Turboprop2::PY_SetBladeTipNode)
            print(text)
        
     
        print ("Pistonprop ( count: ", len(m.pistonprops), "):")
        for item in m.pistonprops:
            text = "\Pistonprop | " 
            text+="\n\t\t reference_node: "+    str(item.reference_node.get_id_str())
            text+="\n\t\t axis_node: "+         str(item.axis_node.get_id_str())
            text+="\n\t\t couple_node: "+       str(item.couple_node.get_id_str())
            text+="\n\t\t _couple_node_set: "+  str(item._couple_node_set)
            text+="\n\t\t turbine_power_kW: "+  str(item.turbine_power_kW)
            text+="\n\t\t pitch: "+             str(item.pitch)
            text+="\n\t\t airfoil: "+           str(item.airfoil)
            # .def("get_blade_tip_node: "+          &Pistonprop::PY_GetBladeTipNode)
            # .def("set_blade_tip_node: "+          &Pistonprop::PY_SetBladeTipNode)        
        
    

    
    # <FIX THE [single fusedrag] FAIL FIRST>
                   
    #    print ("Fusedrag ( count: ", len(m.fusedrag), "):")
    #    for (item in m.turboprops):
    #      text = "\Pistonprop | " 
    #    text+="\n\t\t use_autocalc: "+        str(item.autocalc)
    #    text+="\n\t\t front_node: "+          str(item.front_node.get_id_str())
    #    text+="\n\t\t rear_node: "+           str(item.rear_node.get_id_str())
    #    text+="\n\t\t approximate_width: "+   str(item.approximate_width)
    #    text+="\n\t\t airfoil_name: "+        str(item.airfoil_name)
    #    text+="\n\t\t area_coefficient: "+    str(item.area_coefficient)
    
          
        
        # Hydros
        print ("-- HYDROS --")
        
        def print_command(command):
          text = "\t\tCommand | node1:" + command.node_1.get_id_str()
          text += ", node2:" + command.node_2.get_id_str()
          print(text) 
          
        print("num beam presets: ", len(m.command_hydros_by_preset))
        for group in m.command_hydros_by_preset:
            print("\tBeamPreset (count:", len(group.command_hydros), ")")
            for command in group.command_hydros:
                print_command(command) 
        
        # Nodes
        print("-- NODES --")
        
        def print_node(node):
            text = "\t\tNode | id: " + node.id.to_str()
            text += ", id_num: " + str(node.id.id_num)
            text += ", id_str: " + node.id.id_str
            text += ", x: " + str(node.position.x)
            text += ", flag_n: " + str(node.option_n)
            text += ", flag_l: " + str(node.option_l)
            print(text) 
        
        print("num node presets: ", len(m.nodes_by_preset))
        for node_group in m.nodes_by_preset:
            print("\tNodePreset (count:", len(node_group.nodes), ")")
            for node in node_group.nodes:
                print_node(node)
        
        # Beams
        print("-- BEAMS --")
        
        def print_beam(beam):
          text = "\t\tBeam | node1:" + beam.node_1.get_id_str()
          text += ", node2:" + beam.node_2.get_id_str()
          print(text)  
        
        print("num beam presets: ", len(m.beams_by_preset))
        for beam_group in m.beams_by_preset:
            print("\tBeamPreset (count:", len(beam_group.beams), ")")
            for beam in beam_group.beams:
                print_beam(beam)
                
        
        
        
        
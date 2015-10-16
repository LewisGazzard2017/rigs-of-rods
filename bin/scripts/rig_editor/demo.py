
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
                
        # Hydros
        print ("-- HYDROS --")
        
        def print_command(command):
          text = "\t\tCommand | node1:" + beam.node_1.get_id_str()
          text += ", node2:" + beam.node_2.get_id_str()
          print(text) 
          
        print("num beam presets: ", len(m.command_hydros_by_preset))
        for group in m.command_hydros_by_preset:
            print("\tBeamPreset (count:", len(group.command_hydros), ")")
            for command in group.command_hydros:
                print_command(command) 
        
        
        
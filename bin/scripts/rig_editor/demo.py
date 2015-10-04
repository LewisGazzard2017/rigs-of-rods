
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
            text = "\tNode | id: " + node.id.to_str()
            text += ", id_num: " + str(node.id.id_num)
            text += ", id_str: " + node.id.id_str
            text += ", x: " + str(node.position.x)
            text += ", flag_n: " + str(node.option_n)
            text += ", flag_l: " + str(node.option_l)
            print(text) 
        
        print("num nodes: ", len(m.nodes))
        for node in m.nodes:
            print_node(node)
        # Does property assignment work?
        n = m.nodes[0]
        print("node flag test A: ", n.option_x)
        m.nodes[0].option_x = True
        print("node flag test B: ", m.nodes[0].option_x) 
        # String property test
        print("string property test A: ", n.id.id_str)
        n.id.id_str = "TestPassed!"
        print("string property test B: ", m.nodes[0].id.id_str)
        
        # Beams
        print("-- BEAMS --")
        
        def print_beam(beam):
          text = "\tBeam | node1:" + beam.node_1.get_id_str()
          text += ", node2:" + beam.node_2.get_id_str()
          print(text)  
        
        print("num beams: ", len(m.beams))
        for beam in m.beams:
            print_beam(beam)
        
        
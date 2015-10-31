
# Tests of python bindings

class Demo:

    def draw_demo_mesh():
        import ror_drawing
        from euclid3 import Vector3
        from datatypes import Color
        
        mesh = ror_drawing.create_lines_mesh();
        mesh.set_position(Vector3(0,0,0))
        mesh.begin_update()
        mesh.add_line(Vector3(0, 0, 0), Color.from_rgb(1, 1 ,0),     Vector3(5, 0, 0), Color.from_rgb(1, 0, 0))
        mesh.add_line(Vector3(0, 0, 0), Color.from_rgb(0.2, 0.8 ,1), Vector3(0, 0, 5), Color.from_rgb(0, 0, 1))
        mesh.add_line(Vector3(0, 0, 0), Color.from_rgb(0.1, 1, 0.7), Vector3(0, 5, 0), Color.from_rgb(0, 1, 0))
        mesh.end_update()
        mesh.attach_to_scene()
        
    def test_gui():
        import ror_gui
        root_widgets = ror_gui.load_layout("rig_editor_hydros_panel.layout", "", None)
        print("num root widgets:", len(root_widgets))
        window = root_widgets[0]
        
        
        label = window.find_widget("start_delay_label")
        unicode_str = "%říšěžťčň"
        print("UNICODE:", unicode_str)
        
        # Unicode test
        # IMPORTANT: Make sure MyGUI uses a font with international characters.
        # Review /bin/resources/mygui/MyGUI_FontsEnglish.xml
        label.set_caption(unicode_str)
        out_unicode = label.get_caption()
        print("UNICODE OUTPUT:", out_unicode)
        col = ror_gui.MyGUI_Colour(1.0, 0.0, 1.0, 1.0)
        print("RGBA Colour created")
        col_default = ror_gui.MyGUI_Colour()
        print("default colour created")
        label.set_text_colour(col)
        
        v_checkbox_widget = window.find_widget("flag_v_checkbox")
        #v_checkbox = v_checkbox_widget
        v_checkbox = v_checkbox_widget.cast_type_Button() # Checkbox is not exported yet
        print ("cast done OK")
        obj = {"hello":"RigEditor"}
        def get_widget_name_safe(widget):
            if widget is not None:
                return str(widget.get_name())
            else:
                return "~None~"
        
        def func_set_focus(obj, widget_old, widget_new):
            print("@GotFocus; new widget:", get_widget_name_safe(widget_old), "; old widget:" + get_widget_name_safe(widget_new) )
        v_checkbox.event_mouse_set_focus.add_delegate(obj, func_set_focus)
        
        def func_lost_focus(obj, widget_old, widget_new):
            print("@LostFocus; old widget:", get_widget_name_safe(widget_old), "; new widget:" + get_widget_name_safe(widget_new) )
        v_checkbox.event_mouse_lost_focus.add_delegate(obj, func_lost_focus)
        
        def func_click(obj, widget):
            print("@Click! widget: ", get_widget_name_safe(widget))
        v_checkbox.event_mouse_button_click.add_delegate(obj, func_click)
        
        
        
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
        
        def write_node_preset(obj):
            text="<NodePreset>"
            text+="\n\t\t load_weight :" + str(obj.load_weight)
            text+="\n\t\t friction    :" + str(obj.friction   )
            text+="\n\t\t volume      :" + str(obj.volume     )
            text+="\n\t\t surface     :" + str(obj.surface    )
            text+="\n\t\t option_n    :" + str(obj.option_n   )
            text+="\n\t\t option_m    :" + str(obj.option_m   )
            text+="\n\t\t option_f    :" + str(obj.option_f   )
            text+="\n\t\t option_x    :" + str(obj.option_x   )
            text+="\n\t\t option_y    :" + str(obj.option_y   )
            text+="\n\t\t option_c    :" + str(obj.option_c   )
            text+="\n\t\t option_h    :" + str(obj.option_h   )
            text+="\n\t\t option_e    :" + str(obj.option_e   )
            text+="\n\t\t option_b    :" + str(obj.option_b   )
            text+="\n\t\t option_p    :" + str(obj.option_p   )
            text+="\n\t\t option_L    :" + str(obj.option_L   )
            text+="\n\t\t option_l    :" + str(obj.option_l   )
            return text

        def write_beam_preset(obj):
            text="<BeamPreset>"
            text+="\n\t\t springiness                     :" + str(obj.springiness                    )
            text+="\n\t\t damping_constant                :" + str(obj.damping_constant               )
            text+="\n\t\t deformation_threshold_constant  :" + str(obj.deformation_threshold_constant )
            text+="\n\t\t breaking_threshold_constant     :" + str(obj.breaking_threshold_constant    )
            text+="\n\t\t visual_beam_diameter            :" + str(obj.visual_beam_diameter           )
            text+="\n\t\t plastic_deformation_coefficient :" + str(obj.plastic_deformation_coefficient)
            text+="\n\t\t _enable_advanced_deformation    :" + str(obj._enable_advanced_deformation   )
            
            text+="\n\t\t scale.springiness                     :" + str(obj.scale.springiness                  )
            text+="\n\t\t scale.damping_constant                :" + str(obj.scale.damping_constant             )
            text+="\n\t\t scale.deformation_threshold_constant  :" + str(obj.scale.deformation_threshold_constant)
            text+="\n\t\t scale.breaking_threshold_constant     :" + str(obj.scale.breaking_threshold_constant  )
            return text
            
        # Wheels
        
        print(" -- WHEELS -- ")
        
        def write_wheel_nodes(obj):
            text ="\n\t\t Node1             :" + obj.node_1.get_id_str()
            text+="\n\t\t Node2             :" + obj.node_2.get_id_str()
            return text
        
        def write_base_wheel_attrs(obj):
            text=write_wheel_nodes(obj)
            text+="\n\t\t width             :" + str(obj.width             )
            text+="\n\t\t num_rays          :" + str(obj.num_rays          )
            text+="\n\t\t rigidity_node     :" + obj.rigidity_node.get_id_str()
            text+="\n\t\t braking           :" + str(obj.braking           )
            text+="\n\t\t propulsion        :" + str(obj.propulsion        )
            text+="\n\t\t reference_arm_node:" + obj.reference_arm_node.get_id_str()
            text+="\n\t\t mass              :" + str(obj.mass              )
            text+="\n\t\t node_preset       :" + write_node_preset(obj.node_preset)
            text+="\n\t\t beam_preset       :" + write_beam_preset(obj.beam_preset)
            return text
            
        def write_basewheel2_attrs(obj):
            text=write_wheel_nodes(obj)
            text+="\n\t\t rim_radius        :" + str(obj.rim_radius)
            text+="\n\t\t tyre_radius       :" + str(obj.tyre_radius)
            text+="\n\t\t tyre_springiness  :" + str(obj.tyre_springiness)
            text+="\n\t\t tyre_damping      :" + str(obj.tyre_damping)
            return text
            
        def print_wheel1(obj):
            text="\tWheel:"
            text += write_base_wheel_attrs(obj)
            text+="\n\t\t radius            :" + str(obj.radius            )
            text+="\n\t\t springiness       :" + str(obj.springiness       )
            text+="\n\t\t damping           :" + str(obj.damping           )
            text+="\n\t\t face_material_name:" + str(obj.face_material_name)
            print(text)

        def print_wheel2(obj):
            text="\tWheel2:"
            text += write_basewheel2_attrs(obj)
            text+="\n\t\t face_material_name:" + str(obj.face_material_name)
            text+="\n\t\t band_material_name:" + str(obj.band_material_name)
            text+="\n\t\t rim_springiness   :" + str(obj.rim_springiness   )
            text+="\n\t\t rim_damping       :" + str(obj.rim_damping       )
            print(text)

        def print_meshwheel(obj):
            text="\tMeshWheel:"
            text += write_base_wheel_attrs(obj)
            text+="\n\t\t side              :" + str(obj.side         )
            text+="\n\t\t mesh_name         :" + str(obj.mesh_name    )
            text+="\n\t\t material_name     :" + str(obj.material_name)
            text+="\n\t\t rim_radius        :" + str(obj.rim_radius   )
            text+="\n\t\t tyre_radius       :" + str(obj.tyre_radius  )
            text+="\n\t\t spring            :" + str(obj.spring       )
            text+="\n\t\t damping           :" + str(obj.damping      )
            print(text)
           
        def print_meshwheel2(obj):
            text="\tMeshWheel2:"
            text += write_basewheel2_attrs(obj)
            text+="\n\t\t side              :" + str(obj.side           )
            text+="\n\t\t mesh_name         :" + str(obj.mesh_name      )
            text+="\n\t\t material_name     :" + str(obj.material_name  )
            print(text)
         
        def print_flexbodywheel(obj):
            text="\tFlexBodyWheel:"
            text += write_basewheel2_attrs(obj)
            text+="\n\t\t side              :" + str(obj.side           )
            text+="\n\t\t rim_springiness   :" + str(obj.rim_springiness)
            text+="\n\t\t rim_damping       :" + str(obj.rim_damping    )
            text+="\n\t\t rim_mesh_name     :" + str(obj.rim_mesh_name  )
            text+="\n\t\t tyre_mesh_name    :" + str(obj.tyre_mesh_name )
            print(text)
            
        print ("Wheels (", len(m.wheels), "):")
        for obj in m.wheels:
            print_wheel1(obj)
            
        print ("Wheels2 (", len(m.wheels_2), "):")
        for obj in m.wheels_2:
            print_wheel2(obj)
            
        print ("MeshWheels (", len(m.mesh_wheels), "):")
        for obj in m.mesh_wheels:
            print_meshwheel(obj)
        
        print ("MeshWheels2 (", len(m.mesh_wheels_2), "):")
        for obj in m.mesh_wheels_2:
            print_meshwheel2(obj)
            
        print ("FlexBodyWheels (", len(m.flex_body_wheels), "):") 
        for obj in m.flex_body_wheels:
            print_flexbodywheel(obj)        
        
        # Powertrain
        print (" -- POWERTRAIN -- ")
        
        def print_optional_inertia(obj):
            text = write_optional_inertia(obj)
            print(text)
            
        def write_optional_inertia(obj):
            text = "\tOptionalInertia |"
            text+="\n\t\t start_delay_factor     :" + str(obj.start_delay_factor     )
            text+="\n\t\t stop_delay_factor      :" + str(obj.stop_delay_factor      )
            text+="\n\t\t start_function         :" + str(obj.start_function         )
            text+="\n\t\t stop_function          :" + str(obj.stop_function          )
            text+="\n\t\t _start_delay_factor_set:" + str(obj._start_delay_factor_set)
            text+="\n\t\t _stop_delay_factor_set :" + str(obj._stop_delay_factor_set )
            return text
            
        def print_default_inertia(obj):
            text = "\tOptionalInertia |"
            text+="\n\t\t start_delay_factor"     + str(obj.start_delay_factor     )
            text+="\n\t\t stop_delay_factor"      + str(obj.stop_delay_factor      )
            text+="\n\t\t start_function"         + str(obj.start_function         )
            text+="\n\t\t stop_function "         + str(obj.stop_function          )
            print(text)
        
        if (m.engine is not None):
            text = "\tEngine | ";
            engine = m.engine
            text += "\n\t\t shift_down_rpm      :" + str(engine.shift_down_rpm      )
            text += "\n\t\t shift_up_rpm        :" + str(engine.shift_up_rpm        )
            text += "\n\t\t torque              :" + str(engine.torque              )
            text += "\n\t\t global_gear_ratio   :" + str(engine.global_gear_ratio   )
            text += "\n\t\t reverse_gear_ratio  :" + str(engine.reverse_gear_ratio  )
            text += "\n\t\t neutral_gear_ratio  :" + str(engine.neutral_gear_ratio  )
            # Gears
            text += "\n\t\tGears (" + str(len(m.engine.gear_ratios) ) + "): "
            for ratio in m.engine.gear_ratios:
                text += str(ratio) + " "
            print(text)
        
        if (m.engoption is not None):
            obj = m.engoption
            text = "\tEngoption | "
            text+="\n\t\t type                     :" + str(obj.type                     )
            text+="\n\t\t clutch_force             :" + str(obj.clutch_force             )
            text+="\n\t\t _clutch_force_use_default:" + str(obj._clutch_force_use_default)
            text+="\n\t\t shift_time               :" + str(obj.shift_time               )
            text+="\n\t\t clutch_time              :" + str(obj.clutch_time              )
            text+="\n\t\t post_shift_time          :" + str(obj.post_shift_time          )
            text+="\n\t\t idle_rpm                 :" + str(obj.idle_rpm                 )
            text+="\n\t\t _idle_rpm_use_default    :" + str(obj._idle_rpm_use_default    )
            text+="\n\t\t stall_rpm                :" + str(obj.stall_rpm                )
            text+="\n\t\t max_idle_mixture         :" + str(obj.max_idle_mixture         )
            text+="\n\t\t min_idle_mixture         :" + str(obj.min_idle_mixture         )
            text+="\n\t\t inertia:                 :" + str(obj.min_idle_mixture         )
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
        print ("-- COMMAND HYDROS --")
        
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
                
        
        
        
        
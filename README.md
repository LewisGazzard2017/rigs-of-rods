# Rigs of Rods 
## Editor-Python-Dirty branch

Copyright (c) 2005-2013 Pierre-Michel Ricordel  
Copyright (c) 2007-2013 Thomas Fischer  
Copyright (c) 2013-2015 Petr Ohlidal  

This is Rigs of Rods, an open source vehicle simulator focussed on simulating vehicle physics.

#### About the project:

Alpha prototype of new RigEditor, fully powered by Python 3.4 with bindings to any necessary functionality.

The idea is to open RigEditor to wider developer base and give it a powerful plugin system, such as the one of Blender (www.blender.org)

#### API draft:

Module RoR:

* truck_format_IO: Bindings for RigDef* C++ API for reading/writing files in the classic .truck format.
* GUI: Simplified bindings to MyGUI library for GUI manipulation. Entire editor GUI will be made through this module.
* drawing: Simplified bindings to OGRE engine for creating/manipulating 3D objects. All editor visualizations will be done through this module.
* system: Minimal necessary interaction with the surrounding application.

##### Module "truck_format_IO" (draft)
    class Parser
    Parser.parse(file_path) >> Rig instance
    Parser.get_report() >> List of messages
    class Serializer
    Serializer.serialize(rig_instance, file_path)

##### Module "GUI" (draft)
    
    load_layout(file_path) >> Window -- Loads MyGUI's XML .layout file
    class Window
    Window.find_widget(widget_name) >> Widget
    class Widget
    Widget.bind_event_callback(event_type, callback_func)
    
##### Module "3d_engine" (draft)
    class DynamicMeshOfLines
    class DynamicMeshOfPoints
    DynamicMesh*.begin_init()
    DynamicMesh*.end_init()
    DynamicMesh*.begin_update()
    DynamicMesh*.end_update()
    DynamicMeshOfLines.add_line(position1, color1, position2, color2)
    DynamicMeshOfPoints.add_point(position, color)
    class CameraController
    CameraController.update(move_horizontal, move_vertical, zoom)

##### Module "system" (draft)
    render_one_frame()
    register_input_listener(input_listener_object)
    update_input_and_GUI()
    show_open_save_file_dialog(mode, finished_callback)
    
    
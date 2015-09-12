# Rigs of Rods 
## Editor-Python-Dirty branch

Alpha prototype of new RigEditor, fully powered by Python 3.4 with bindings to any necessary functionality.

The idea is to open RigEditor to wider developer base and give it a powerful plugin system, such as the one of Blender (www.blender.org)

The 'dirty' word in the name refers to code-quality: Python bindings are made 
from former AngelScript bindings (also only experimental) and obsoleted C++ 
implementations are left lying all over the place. When the code matures, 
it will be carefully cherry-picked to master branch.

#### Status

Working test RoR launches "scripts/rig_editor/Main.py" which
1. sets up scene/camera; 
2. renders one frame (dark purple background); 
3. speeps for 5 sec; 
4. exits

#### API Draft

##### Module "ror_truck_fileformat" (draft)
Bindings for RigDef* C++ API for reading/writing files in the classic .truck format.

    class Parser
        Parser.parse(file_path) >> Rig instance
        Parser.get_report() >> List of messages
    class Serializer
        Serializer.serialize(rig_instance, file_path)

##### Module "ror_gui" (draft)
Simplified bindings to MyGUI library for GUI manipulation. Entire editor GUI will be made through this module.

    load_layout(file_path) >> Window -- Loads MyGUI's XML .layout file
    class Window
        Window.find_widget(widget_name) >> Widget
    class Widget
        Widget.bind_event_callback(event_type, callback_func)
    
##### Module "ror_drawing" (draft)
Simplified bindings to OGRE engine for creating/manipulating 3D objects. All editor visualizations will be done through this module.

    class DynamicMeshOfLines
    class DynamicMeshOfPoints
        DynamicMesh*.begin_init()
        DynamicMesh*.end_init()
        DynamicMesh*.begin_update()
        DynamicMesh*.end_update()
        DynamicMeshOfLines.add_line(position1, color1, position2, color2)
        DynamicMeshOfPoints.add_point(position, color)
    class CameraController
        CameraController.set_rotation_center(position)
        CameraController.get_rotation_center(position)
        CameraController.update(move_horizontal, move_vertical, zoom)

##### Module "ror_system" (draft)
Minimal necessary interaction with the surrounding application.

    [DONE] enter_rig_editor()
    [DONE] render_frame_and_update_window()
    register_input_listener(input_listener_object)
    update_input_and_GUI()
    show_open_save_file_dialog(mode, finished_callback)
    
    
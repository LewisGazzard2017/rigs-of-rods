# Rigs of Rods 
## Editor-Python-Dirty branch

Alpha prototype of new RigEditor, fully powered by Python 3.4 with bindings to any necessary functionality.

The idea is to open RigEditor to wider developer base and give it a powerful plugin system, such as the one of Blender (www.blender.org)

The 'dirty' word in the name refers to code-quality: Python bindings are made 
from former AngelScript bindings (also only experimental) and obsoleted C++ 
implementations are left lying all over the place. When the code matures, 
it will be carefully cherry-picked to master branch.

#### Status

Working technical preview:
 1. sets up scene/camera (dark purple background);
 2. draws one LinesMesh with 3 axis-lines. 
 3. stays idle until you pres ESC key to exit to main menu.
 4. user may enter RigEditor any number of times.

#### API Draft

##### Module "ror_truck_fileformat" (draft)
Bindings for RigDef* C++ API for reading/writing files in the classic .truck format.

    class Parser
        Parser.parse(file_path) >> Rig instance
        Parser.get_report() >> List of messages
    class Serializer
        Serializer.serialize(rig_instance, file_path)

##### Module "ror_gui"
Simplified bindings to MyGUI library for GUI manipulation. Entire editor GUI will be made through this module.

    (draft) load_layout(file_path) >> Window -- Loads MyGUI's XML .layout file
    (draft) class Window
        (draft) Window.find_widget(widget_name) >> Widget
    (draft) class Widget
        (draft) Widget.bind_event_callback(event_type, callback_func)
    
##### Module "ror_drawing"
Simplified bindings to OGRE engine for creating/manipulating 3D objects. All editor visualizations will be done through this module.

    [DONE] class LinesMesh
    [DONE] class PointsMesh
        [DONE] *Mesh.begin_update()
        [DONE] *Mesh.end_update()
        [DONE] LinesMesh.add_line(position1, color1, position2, color2)
        [DONE] PointsMesh.add_point(position, color)
    (draft) class CameraController
        (draft) CameraController.set_rotation_center(position)
        (draft) CameraController.get_rotation_center(position)
        (draft) CameraController.update(move_horizontal, move_vertical, zoom)

##### Module "ror_system"
Minimal necessary interaction with the surrounding application.

    [DONE] enter_rig_editor()
    [DONE] render_frame_and_update_window()
    [DONE] is_application_window_closed()
    [DONE] request_application_shutdown()
    [DONE] set_input_listener()
    (draft) register_input_listener(input_listener_object)
    (draft) update_input_and_GUI()
    (draft) show_open_save_file_dialog(mode, finished_callback)
    
    

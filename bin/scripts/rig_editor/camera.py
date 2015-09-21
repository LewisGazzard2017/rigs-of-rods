
from datatypes import Vector3

class CameraOrbitController:
    'Mouse-controlled camera orbiting around a defined point'

    def __init__(self, camera, ortho_zoom_ratio):
        self.camera = camera
        camera.set_position(Vector3(10,5,10)) # Initial pos. TODO: Make configurable
        self.orbit_target = Vector3(0, 0, 0)
        self.ortho_zoom_ratio = ortho_zoom_ratio
        self.is_zooming = False
        
    def update_ortho_zoom(self):
        target_z_distance = self.camera.get_point_z_distance(self.orbit_target)
        ortho_window_width = target_z_distance * self.ortho_zoom_ratio
        self.camera.set_ortho_window_width(ortho_window_width)
      
    def inject_mouse_move(self, do_orbit, x_rel, y_rel, wheel_rel):
        z_distance = self.camera.get_point_z_distance(self.orbit_target)
        if (do_orbit):
            self.camera.set_position(target)
            self.camera.yaw_degrees  (-x_rel * 0.25)
            self.camera.pitch_degrees(-y_rel * 0.25)
            self.camera.move_relative(Vector3(0, 0, z_distance))
        elif (self.is_zooming): 
            # (mouse-move zooming) move the camera toward or away from the target
            # the further the camera is, the faster it moves
            self.camera.move_relative(Vector3(0, 0, y_rel * 0.004 * dist))
        elif (wheel_rel != 0): 
            # move the camera toward or away from the target
            offset = Vector3(0, 0, -wheel_rel * 0.0008 * dist)
            self.camera.move_relative(offset)
            if (self.camera.is_mode_ortho()):
                self.update_ortho_zoom()
        
            
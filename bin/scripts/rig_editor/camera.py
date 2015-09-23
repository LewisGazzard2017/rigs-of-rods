
from euclid3 import Vector3

class CameraOrbitController:
    'Mouse-controlled camera orbiting around a defined point'

    def __init__(self, camera, ortho_zoom_ratio):
        self.camera = camera
        self.orbit_target = Vector3(0, 0, 0)
        self.ortho_zoom_ratio = ortho_zoom_ratio
        self.is_zooming = False
        self.camera_distance = 0
        
        camera.set_position(Vector3(10,5,10)) # Initial pos. TODO: Make configurable
        camera.look_at(self.orbit_target)
        # TODO - IMPORTANT
        # Investigate why result of {camera.get_point_z_distance()}
        # may be slightly inaccurate (too high) depending on camera angle.
        # Observed when using this funcion to compute distance each frame:
        # under most angles, the camera was moving away from the target.
        self.camera_distance = camera.get_point_z_distance(self.orbit_target)
        
    def update_ortho_zoom(self):
        ortho_window_width = self.camera_distance * self.ortho_zoom_ratio
        self.camera.set_ortho_window_width(ortho_window_width)
      
    def inject_mouse_move(self, do_orbit, x_rel, y_rel, wheel_rel):
        if (do_orbit):
            self.camera.set_position(self.orbit_target)
            self.camera.yaw_degrees  (-x_rel * 0.25)
            self.camera.pitch_degrees(-y_rel * 0.25)
            self.camera.move_relative(Vector3(0, 0, self.camera_distance))
        elif (self.is_zooming): 
            # (mouse-move zooming) move the camera toward or away from the target
            # the further the camera is, the faster it moves
            self.camera_distance += (y_rel * 0.004 * self.camera_distance)
            self.camera.set_position(self.orbit_target)
            self.camera.move_relative(Vector3(0, 0, self.camera_distance))
        elif (wheel_rel != 0): 
            # move the camera toward or away from the target
            self.camera_distance += (wheel_rel * 0.0008 * self.camera_distance)
            self.camera.set_position(self.orbit_target)
            self.camera.move_relative(Vector3(0, 0, self.camera_distance))
            if (self.camera.is_mode_ortho()):
                self.update_ortho_zoom()
        
            
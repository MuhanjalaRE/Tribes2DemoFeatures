#pragma once

namespace t2 {
	namespace settings {
		extern float camera_move_speed_xy;
		extern float camera_move_speed_z;
		extern float camera_rotation_speed_pitch;
		extern float camera_rotation_speed_yaw;
		extern float camera_fov;
		extern bool set_camera;
		void LoadSettings(void);
	}
}
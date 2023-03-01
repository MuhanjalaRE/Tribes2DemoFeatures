#pragma once

namespace t2 {
	namespace settings {
		extern float camera_move_speed_xy;
		extern float camera_move_speed_z;
		extern float camera_rotation_speed_pitch;
		extern float camera_rotation_speed_yaw;
		extern int camera_fov;
		//extern bool set_camera;
		extern bool show_player_model;
		extern bool show_weapon_model;
		extern float third_person_zoom_rate;
		void LoadSettings(void);
	}
}
#include "settings.h"
#include <json/json.hpp>
#include <fstream>

namespace t2 {
	namespace settings {
		float camera_move_speed = 0.12;
		float camera_rotation_speed = 0.00075;
		float camera_fov = 120;

		void LoadSettings(void) {
			std::ifstream ifs("demo_settings.json");
			nlohmann::json settings_json = nlohmann::json::parse(ifs);
			nlohmann::json camera_json = settings_json["camera"];

			camera_move_speed = camera_json["move_speed"].get<float>();
			camera_rotation_speed = camera_json["rotation_speed"].get<float>();
			camera_fov = camera_json["fov"].get<float>();
		}
	}
}
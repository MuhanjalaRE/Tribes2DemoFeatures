#pragma once

#include <t2/abstraction/Camera.h>

namespace t2 {
	namespace game_data {
		namespace demo {
			//extern t2::abstraction::Camera camera;
			extern void* game_connection;
			extern void* player;
			extern void* camera;
			enum class ViewTarget{kUnknown = 0, kCamera = 1, kPlayer = 2};
			extern ViewTarget view_target;
			extern bool is_player_alive;
			void ToggleViewTarget(void);
			void ToggleRecording(void);
		}
	}
}
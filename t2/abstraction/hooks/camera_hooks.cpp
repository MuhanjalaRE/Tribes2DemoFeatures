#include <t2/abstraction/Camera.h>
#include <t2/game data/demo.h>

namespace t2 {
	namespace abstraction {
		namespace hooks {
			namespace Camera {
				SetPosition OriginalSetPosition = (SetPosition)0x5CC270;
				void __fastcall SetPositionHook(void* this_camera, void* _, void* position, void* rotation) {
					if (!t2::game_data::demo::is_player_alive && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kPlayer)
						OriginalSetPosition(this_camera, position, rotation);
					return;
				}

				ProcessTick OriginalProcessTick = (ProcessTick)0x5CBC80;
				void __fastcall ProcessTickHook(void* this_camera, void* _, void* move) {
					return;
				}

				SetFlyMode OriginalSetFlyMode = (SetFlyMode)0x005CCCE0;
			}
		}
	}
}
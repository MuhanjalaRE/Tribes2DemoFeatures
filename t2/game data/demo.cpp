#include <t2/game data/demo.h>
#include <t2/abstraction/GameConnection.h>
#include <t2/abstraction/NetConnection.h>

namespace t2 {
	namespace game_data {
		namespace demo {
			//t2::abstraction::Camera camera(NULL);
			void* game_connection = NULL;
			void* player = NULL;
			void* camera = NULL;
			ViewTarget view_target = ViewTarget::kCamera;
			bool is_player_alive = false;

			void ToggleViewTarget(void){
				if (!game_connection)
					return;

				if (view_target == ViewTarget::kCamera){
					// We set the view_target to be a player
					view_target = ViewTarget::kPlayer;
					// If the player is alive then immediately set it as the control object. Otherwise when ReadPacket -> SetControlObject is called it will see that our view target has been set to a player and will then change the control object to the player
					if (is_player_alive && player){
						t2::abstraction::hooks::GameConnection::OriginalSetControlObject(game_connection, player);
					}
				} else if (view_target == ViewTarget::kPlayer){
					view_target = ViewTarget::kCamera;
					// When the view target is a camera and if a camera has been found then immediately set it as the control object. If it hasn't been found then when the player dies SetControlObject is called to set a camera object as the control object
					if (camera){
						t2::abstraction::hooks::GameConnection::OriginalSetControlObject(game_connection, camera);
					}
				}
			}

			void ToggleRecording(void) {
				static bool is_recording = false;

				if (!game_connection)
					return;

				is_recording = !is_recording;

				if (is_recording) {
					t2::abstraction::hooks::NetConnection::StartDemoRecordHook(game_connection, NULL, "recordings/test.rec");
				}
				else {
					t2::abstraction::hooks::NetConnection::StopDemoRecordHook(game_connection, NULL);
				}
			}
		}
	}
}
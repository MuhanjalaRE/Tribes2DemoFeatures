#include <t2/game data/demo.h>
#include <t2/abstraction/GameConnection.h>
#include <t2/abstraction/NetConnection.h>
#include <Windows.h>

namespace t2 {
	namespace game_data {
		namespace demo {
			//t2::abstraction::Camera camera(NULL);
			void* game_connection = NULL;
			void* player = NULL;
			void* camera = NULL;
			ViewTarget view_target = ViewTarget::kPlayer;
			bool is_player_alive = false;
			t2::math::Matrix player_matrix;
			t2::math::Matrix camera_matrix;
			t2::math::Vector camera_position;
			t2::math::Vector camera_direction;
			t2::math::Vector camera_rotation;
			bool initialised = false;
			float camera_yaw_offset = 0*1.875;
			bool show_iffs = true;
			bool first_person = true;
			float third_person_distance = 10;
			float speed_hack_scale = 1.0f;
			float debug_third_person_offset_scalar = -0.194;
			float fps = *(float*)0x0083F380;
			float zoom_fov_delta_per_second = 120;
			float max_fov = 179;
			float min_fov = 1;
			/*CameraMovementState camera_movement_state = CameraMovementState::kStopped;
			LARGE_INTEGER camera_movement_start_time = {0};
			CameraMovementDirection camera_movement_direction = CameraMovementDirection::kNone;
			float camera_movement_acceleration = 1.0f;
			extern float maximum_camera_velocity = 0.33;
			*/

			CameraAxisMovement camera_axis_movement_x;
			CameraAxisMovement camera_axis_movement_y;
			CameraAxisMovement camera_axis_movement_z;

			void ToggleViewTarget(void){
				if (!game_connection)
					return;

				if (view_target == ViewTarget::kCamera){
					// We set the view_target to be a player
					view_target = ViewTarget::kPlayer;
					PLOG_DEBUG << "Viewing player";
					// If the player is alive then immediately set it as the control object. Otherwise when ReadPacket -> SetControlObject is called it will see that our view target has been set to a player and will then change the control object to the player
					/*
					if (is_player_alive && player){
						t2::abstraction::hooks::GameConnection::OriginalSetControlObject(game_connection, player);
					}
					*/
				} else if (view_target == ViewTarget::kPlayer){
					// FLICKER OCCURS IN HERE
					view_target = ViewTarget::kCamera;

					if (!t2::game_data::demo::is_player_alive){
						t2::game_data::demo::camera_position = t2::game_data::demo::player_matrix.GetColumn(3);
						//t2::abstraction::hooks::Camera::OriginalSetPosition(t2::game_data::demo::camera, &t2::game_data::demo::camera_position, &t2::game_data::demo::camera_rotation);
						//PLOG_DEBUG << "1Setting camera position to " << t2::game_data::demo::camera_position.x_ << " " << t2::game_data::demo::camera_position.y_ << " " << t2::game_data::demo::camera_position.z_;
					}
					//t2::game_data::d
					else {
						t2::abstraction::SceneObject player_scene_object(t2::game_data::demo::player);
						t2::game_data::demo::camera_position = player_scene_object.object_to_world_->GetColumn(3);

						t2::math::Matrix eye_matrix;
						//t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::player, &eye_matrix);
						//t2::game_data::demo::camera_position = eye_matrix.GetColumn(3);

						PLOG_DEBUG << "2Setting camera position to " << t2::game_data::demo::camera_position.x_ << " " << t2::game_data::demo::camera_position.y_ << " " << t2::game_data::demo::camera_position.z_;

						t2::math::Vector rot_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, t2::game_data::demo::player, 2380);
						t2::math::Vector head_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, t2::game_data::demo::player, 2368);

						t2::abstraction::hooks::GameConnection::OriginalGetControlCameraTransform(t2::game_data::demo::game_connection, 0, &eye_matrix);

						t2::game_data::demo::camera_position = eye_matrix.GetColumn(3);
						//head_.z_ += PI/2;
						rot_.z_ += camera_yaw_offset;
						rot_.x_ = head_.x_;
						t2::game_data::demo::camera_rotation = rot_;

						/*
						Prevent flickering when switching
						*/
						t2::math::Matrix target_matrix;
						t2::math::Matrix xRot, zRot;
						xRot.Set(t2::math::Vector(t2::game_data::demo::camera_rotation.x_, 0, 0));
						zRot.Set(t2::math::Vector(0, 0, t2::game_data::demo::camera_rotation.z_));
						t2::math::Matrix temp;
						temp.Mul(zRot, xRot);
						temp.SetColumn(3, t2::game_data::demo::camera_position);
						target_matrix = temp;

						t2::abstraction::hooks::SceneObject::OriginalSetRenderTransform(t2::game_data::demo::player, &target_matrix);
						t2::game_data::demo::camera_matrix = target_matrix;
						//t2::abstraction::hooks::Camera::OriginalSetPosition(t2::game_data::demo::camera, &t2::game_data::demo::camera_position, &rot_);

					}

					// When the view target is a camera and if a camera has been found then immediately set it as the control object. If it hasn't been found then when the player dies SetControlObject is called to set a camera object as the control object
					/*
					if (camera){
						t2::abstraction::hooks::GameConnection::OriginalSetControlObject(game_connection, camera);
					}
					*/
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
#pragma once

#include <t2/abstraction/Camera.h>
#include <t2/math.h>
#include <Windows.h>

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
			extern t2::math::Matrix player_matrix;
			extern t2::math::Matrix camera_matrix;
			extern t2::math::Vector camera_position;
			extern t2::math::Vector camera_direction;
			extern t2::math::Vector camera_rotation;
			extern float camera_yaw_offset;
			extern bool initialised;
			extern bool show_iffs;
			extern bool first_person;
			extern float third_person_distance;
			extern float speed_hack_scale;
			extern float debug_third_person_offset_scalar;
			extern float fps;
			extern float zoom_fov_delta_per_second;
			extern float max_fov;
			extern float min_fov;
			//enum class CameraMovementState {kAccelerating, kDecelerating, kStopped};
			//enum class CameraMovementDirection {kForward = 0x57, kBackward= 0x53, kLeft= 0x41, kRight= 0x44, kNone = 0};
			//extern CameraMovementDirection camera_movement_direction;
			//extern CameraMovementState camera_movement_state;
			//extern LARGE_INTEGER camera_movement_start_time;
			//extern float camera_movement_acceleration;
			//extern float maximum_camera_velocity;

			struct CameraAxisMovement{
				enum class State {kStopped, kAccelerating, kDecelerating};
				enum class Direction {kNone, kPositive, kNegative};

				State state = State::kStopped;
				Direction direction = Direction::kNone;
				LARGE_INTEGER acceleration_timestamp = {0};
				LARGE_INTEGER deceleration_timestamp = {0};
				float acceleration_per_second = 1.0;
				float deceleration_per_second = 2.0;
				float maximum_velocity = 0.5;
				float minimum_velocity = 0.0;
				float current_velocity = 0.0;
				float velocity_before_deceleration = 0.0;
			};

			 extern CameraAxisMovement camera_axis_movement_x;
			 extern CameraAxisMovement camera_axis_movement_y;
			 extern CameraAxisMovement camera_axis_movement_z;
		}
	}
}
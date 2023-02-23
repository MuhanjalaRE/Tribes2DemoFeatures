#include <t2/abstraction/ShapeBase.h>
#include <t2/math.h>
#include <t2/game data/player.h>
#include <string>
#include <keys/keys.h>
#include <t2/abstraction/Camera.h>
#include <t2/settings/settings.h>
#include <t2/game data/demo.h>

namespace t2 {
	namespace hooks {
		namespace ShapeBase {
			GetEyeTransform OriginalGetEyeTransform = (GetEyeTransform)0x5eb2c0;
			// Fairly sure in demos this function will only be called the camera. Even if other objects call it, so what? We'll check in the function if the calling object is infact a camera. I THINK this is called once every frame
			void __fastcall GetEyeTransformHook(void* this_shapebase, void* _, t2::math::Matrix* out_matrix) {

				// [IGNORE - read update] If view_target is not a camera then just return the original GetEyeTransform
				// [Update] So if we set our view_target to a player while the view target was previously a camera and the player still hasn't spawned in yet then the camera won't be able to be updated. Maybe this is fine. Maybe this is not fine. For now, let's just allow the camera
				// [Update 2] Let's just disable the camera transform modifications if we've changed the view target to player and the player is currently dead. Shouldn't be a big deal
				if (t2::game_data::demo::view_target != t2::game_data::demo::ViewTarget::kCamera){
					return OriginalGetEyeTransform(this_shapebase, out_matrix);
				}

				t2::abstraction::ShapeBase shapebase_object(this_shapebase);
				if (!shapebase_object.namespace_name_ || std::string(shapebase_object.namespace_name_) != "Camera") {
					return OriginalGetEyeTransform(this_shapebase, out_matrix);
				}

				t2::abstraction::Camera camera_object(this_shapebase);
				t2::abstraction::hooks::Camera::OriginalSetFlyMode(this_shapebase);

				static t2::math::Vector m_rot = *camera_object.m_rot_;
				static const float max_pitch = 1.3962;

				if (keys::mouse_states[3] >= 1) {
					m_rot.x_ += settings::camera_rotation_speed_pitch * keys::mouse_states[3];
				}
				else if (keys::key_states[0x49]) {
					m_rot.x_ += settings::camera_rotation_speed_pitch;
				}

				if (keys::mouse_states[3] <= -1) {
					m_rot.x_ -= settings::camera_rotation_speed_pitch * abs(keys::mouse_states[3]);
				}
				else if (keys::key_states[0x4B]) {
					m_rot.x_ -= settings::camera_rotation_speed_pitch;
				}

				if (m_rot.x_ > max_pitch)
					m_rot.x_ = max_pitch;
				else if (m_rot.x_ < -max_pitch)
					m_rot.x_ = -max_pitch;

				if (keys::mouse_states[2] >= 1) {
					m_rot.z_ += settings::camera_rotation_speed_yaw * keys::mouse_states[2];
				}
				else if (keys::key_states[0x4C]) {
					m_rot.z_ += settings::camera_rotation_speed_yaw;
				}

				if (keys::mouse_states[2] <= -1) {
					m_rot.z_ -= settings::camera_rotation_speed_yaw * abs(keys::mouse_states[2]);
				}
				else if (keys::key_states[0x4A]) {
					m_rot.z_ -= settings::camera_rotation_speed_yaw;
				}

				static t2::math::Vector position = camera_object.object_to_world_->GetColumn(3);
				static t2::math::Vector direction = camera_object.object_to_world_->GetColumn(3);


				if (keys::key_states[0x57]) {
					direction.z_ = 0;
					position += direction.Unit() * settings::camera_move_speed_xy;
				}
				if (keys::key_states[0x53]) {
					direction.z_ = 0;
					position += direction.Unit() * -1 * settings::camera_move_speed_xy;
				}
				if (keys::key_states[0x41]) {
					direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit(); // right vector
					position += direction.Unit() * -1 * settings::camera_move_speed_xy;

				}
				if (keys::key_states[0x44]) {
					direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit(); // right vector
					position += direction.Unit() * settings::camera_move_speed_xy;

				}

				if (keys::mouse_states[0]) {
					direction = t2::math::Vector(0, 0, 1).Unit();
					position += direction.Unit() * -1 * settings::camera_move_speed_z;
				}
				if (keys::mouse_states[1]) {
					direction = t2::math::Vector(0, 0, 1).Unit();
					position += direction.Unit() * 1 * settings::camera_move_speed_z;
				}

				t2::abstraction::hooks::Camera::OriginalSetPosition(this_shapebase, &position, &m_rot);
				OriginalGetEyeTransform(this_shapebase, out_matrix);
				direction = shapebase_object.object_to_world_->GetColumn(1);
			}
		}
	}
}
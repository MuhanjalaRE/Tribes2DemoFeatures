#include <t2/abstraction/ShapeBase.h>
#include <t2/math.h>
#include <t2/game data/player.h>
#include <string>
#include <keys/keys.h>
#include <t2/abstraction/Camera.h>
#include <t2/settings/settings.h>

namespace t2 {
	namespace hooks {
		namespace ShapeBase {
			GetEyeTransform OriginalGetEyeTransform = (GetEyeTransform)0x5eb2c0;
			void __fastcall GetEyeTransformHook(void* this_shapebase, void* _, t2::math::Matrix* out_matrix) {
				t2::abstraction::ShapeBase shapebase_object(this_shapebase);
				if (!shapebase_object.namespace_name_ || std::string(shapebase_object.namespace_name_) != "Camera") {
					return OriginalGetEyeTransform(this_shapebase, out_matrix);
				}

				t2::abstraction::Camera camera_object(this_shapebase);
				t2::abstraction::hooks::Camera::OriginalSetFlyMode(this_shapebase);

				static t2::math::Vector m_rot = *camera_object.m_rot_;
				static const float max_pitch = 1.3962;

				if (keys::mouse_states[3] >= 1) {
					m_rot.x_ += settings::camera_rotation_speed * keys::mouse_states[3];
				}

				if (keys::mouse_states[3] <= -1) {
					m_rot.x_ -= settings::camera_rotation_speed * abs(keys::mouse_states[3]);
				}

				if (m_rot.x_ > max_pitch)
					m_rot.x_ = max_pitch;
				else if (m_rot.x_ < -max_pitch)
					m_rot.x_ = -max_pitch;

				if (keys::mouse_states[2] >= 1) {
					m_rot.z_ += settings::camera_rotation_speed * keys::mouse_states[2];
				}

				if (keys::mouse_states[2] <= -1) {
					m_rot.z_ -= settings::camera_rotation_speed * abs(keys::mouse_states[2]);
				}

				static t2::math::Vector position = camera_object.object_to_world_->GetColumn(3);
				static t2::math::Vector direction = camera_object.object_to_world_->GetColumn(3);


				if (keys::key_states[0x57]) {
					position += direction.Unit() * settings::camera_move_speed;

				}
				if (keys::key_states[0x53]) {
					position += direction.Unit() * -1 * settings::camera_move_speed;
				}
				if (keys::key_states[0x41]) {
					direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit(); // right vector
					position += direction.Unit() * -1 * settings::camera_move_speed;

				}
				if (keys::key_states[0x44]) {
					direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit(); // right vector
					position += direction.Unit() * settings::camera_move_speed;

				}

				if (keys::mouse_states[0]) {
					direction = t2::math::Vector(0, 0, 1).Unit();
					position += direction.Unit() * -1 * settings::camera_move_speed;
				}
				if (keys::mouse_states[1]) {
					direction = t2::math::Vector(0, 0, 1).Unit();
					position += direction.Unit() * 1 * settings::camera_move_speed;
				}

				t2::abstraction::hooks::Camera::OriginalSetPosition(this_shapebase, &position, &m_rot);
				OriginalGetEyeTransform(this_shapebase, out_matrix);
				direction = shapebase_object.object_to_world_->GetColumn(1);
			}
		}
	}
}
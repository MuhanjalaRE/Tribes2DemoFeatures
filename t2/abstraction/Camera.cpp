#include "Camera.h"

namespace t2 {
	namespace abstraction {
		Camera::Camera(void* pointer_to_torque_camera_class) : Parent(pointer_to_torque_camera_class){
			if (!pointer_to_torque_camera_class)
				return;

			m_rot_ = (t2::math::Rotation*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(pointer_to_torque_camera_class, 2200);
		}
		
	}
}
#include "SceneObject.h"
#include <t2/math.h>

namespace t2 {
	namespace abstraction {
		SceneObject::SceneObject(void* pointer_to_torque_sceneobject_class) : Parent(pointer_to_torque_sceneobject_class) {
			if (!pointer_to_torque_sceneobject_class)
				return;

			object_to_world_ = (t2::math::Matrix*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(pointer_to_torque_sceneobject_class, 156);
		}
	}
}
#pragma once

#include "NetObject.h"
#include <t2/math.h>

namespace t2 {
	namespace abstraction {
		class SceneObject : public NetObject {
			typedef NetObject Parent;
		public:
			t2::math::Matrix* object_to_world_ = NULL;
			SceneObject(void* pointer_to_torque_sceneoject_class);
		};
	}
}
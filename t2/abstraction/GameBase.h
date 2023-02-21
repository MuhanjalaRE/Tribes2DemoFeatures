#pragma once

#include "SceneObject.h"

namespace t2 {
	namespace abstraction {
		class GameBase : public SceneObject {
			typedef SceneObject Parent;
		public:
			void* controlling_client_ = NULL;
			GameBase(void* pointer_to_torque_gamebase_class);
		};
	}
}
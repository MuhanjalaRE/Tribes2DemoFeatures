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

		namespace hooks {
			namespace SceneObject {
				void __fastcall SetRenderTransformHook(void* this_sceneobject, void* _, t2::math::Matrix* arg2);
				typedef void(__thiscall* SetRenderTransform)(void*, t2::math::Matrix*);
				extern SetRenderTransform OriginalSetRenderTransform;
			}
		}
	}
}
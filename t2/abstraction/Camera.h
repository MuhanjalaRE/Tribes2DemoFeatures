#pragma once

#include "ShapeBase.h"
#include <t2/math.h>

namespace t2 {
	namespace abstraction {
		class Camera : public ShapeBase {
			typedef ShapeBase Parent;
		public:
			t2::math::Rotation* m_rot_;
			Camera(void* pointer_to_torque_camera_class);
		};

		namespace hooks {
			namespace Camera {
				typedef void(__thiscall* SetPosition)(void*, void*, void*);
				extern SetPosition OriginalSetPosition;

				typedef void(__thiscall* ProcessTick)(void*, void*);
				extern ProcessTick OriginalProcessTick;
				void __fastcall ProcessTickHook(void*, void*, void*);

				typedef void(__thiscall* SetFlyMode)(void*);
				extern SetFlyMode OriginalSetFlyMode;
			}
		}
	}
}
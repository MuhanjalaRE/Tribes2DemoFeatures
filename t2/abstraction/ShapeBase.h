#pragma once

#include "GameBase.h"

namespace t2 {
	namespace abstraction {
		class ShapeBase : public GameBase {
			typedef GameBase Parent;
		public:
			ShapeBase(void* pointer_to_torque_shapebase_class);
		};
	}

	namespace hooks {
		namespace ShapeBase {
			void __fastcall GetEyeTransformHook(void*, void*, t2::math::Matrix*);
			typedef void(__thiscall* GetEyeTransform)(void*, t2::math::Matrix*);
			extern GetEyeTransform OriginalGetEyeTransform;
		}
	}
}
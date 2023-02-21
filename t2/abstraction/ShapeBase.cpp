#include "ShapeBase.h"

namespace t2 {
	namespace abstraction {
		ShapeBase::ShapeBase(void* pointer_to_torque_shapebase_class) : Parent(pointer_to_torque_shapebase_class) {
			if (!pointer_to_torque_shapebase_class)
				return;
		}
	}
}
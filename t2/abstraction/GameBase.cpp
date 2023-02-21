#include "GameBase.h"

namespace t2 {
	namespace abstraction {
		GameBase::GameBase(void* pointer_to_torque_gamebase_class) : Parent(pointer_to_torque_gamebase_class) {
			if (!pointer_to_torque_gamebase_class)
				return;

			controlling_client_ = GET_OBJECT_VARIABLE_BY_OFFSET(void*, pointer_to_torque_gamebase_class, 181 * 4);
		}
	}
}
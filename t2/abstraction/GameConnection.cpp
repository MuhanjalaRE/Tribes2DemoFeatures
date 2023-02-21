#include "GameConnection.h"

namespace t2 {
	namespace abstraction {
		GameConnection::GameConnection(void* pointer_to_torque_gameconnection_class) : Parent(pointer_to_torque_gameconnection_class) {
			if (!pointer_to_torque_gameconnection_class)
				return;

			controlling_object_ = GET_OBJECT_VARIABLE_BY_OFFSET(int*, pointer_to_torque_gameconnection_class, 33372);
			damage_flash_= (float*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(pointer_to_torque_gameconnection_class, 8466 * 4);
		}
	}
}
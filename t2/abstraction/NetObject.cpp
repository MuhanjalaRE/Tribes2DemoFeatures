#include "NetObject.h"

namespace t2 {
	namespace abstraction {
		NetObject::NetObject(void* pointer_to_torque_netobject_class) : Parent(pointer_to_torque_netobject_class) {
			if (!pointer_to_torque_netobject_class)
				return;
		}
	}
}
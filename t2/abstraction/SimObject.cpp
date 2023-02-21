#include "SimObject.h"
#include "Player.h"

namespace t2 {
	namespace abstraction {
		SimObject::SimObject(void* pointer_to_torque_simobject_class) {
			if (!pointer_to_torque_simobject_class)
				return;

			pointer_to_torque_simobject_class_ = pointer_to_torque_simobject_class;
			id_ = GET_OBJECT_VARIABLE_BY_OFFSET(unsigned int, pointer_to_torque_simobject_class, 32);
			void* namespace__ = GET_OBJECT_VARIABLE_BY_OFFSET(void*, pointer_to_torque_simobject_class, 36);
			if (namespace__) {
				namespace_name_ = GET_OBJECT_VARIABLE_BY_OFFSET(char*, namespace__, 0);
			}
		}
	}
}
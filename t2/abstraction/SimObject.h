#pragma once

#include "Helper.h"

namespace t2 {
	namespace abstraction {
		class SimObject {
		public:
			void* pointer_to_torque_simobject_class_ = NULL;
			unsigned int id_ = -1;
			char* namespace_name_ = NULL;
			SimObject(void* pointer_to_torque_simobject_class);
		};
	}
}


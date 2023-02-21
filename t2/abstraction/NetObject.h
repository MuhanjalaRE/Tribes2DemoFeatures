#pragma once

#include "SimObject.h"

namespace t2 {
	namespace abstraction {
		class NetObject : public SimObject {
			typedef SimObject Parent;
		public:
			NetObject(void* pointer_to_torque_netobject_class);
		};
	}
}
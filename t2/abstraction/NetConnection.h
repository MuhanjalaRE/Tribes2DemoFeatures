#pragma once

#include "Helper.h"

namespace t2 {
	namespace abstraction {
		class NetConnection {
		public:
			void* pointer_to_torque_netconnection_class_ = NULL;
			NetConnection(void* pointer_to_torque_netconnection_class);
		};
	}
}
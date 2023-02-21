#include "NetConnection.h"

namespace t2 {
	namespace abstraction {
		NetConnection::NetConnection(void* pointer_to_torque_netconnection_class) {
			if (!pointer_to_torque_netconnection_class)
				return;

			pointer_to_torque_netconnection_class_ = pointer_to_torque_netconnection_class;
		}
	}
}
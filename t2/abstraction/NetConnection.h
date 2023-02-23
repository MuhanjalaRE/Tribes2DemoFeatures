#pragma once

#include "Helper.h"

namespace t2 {
	namespace abstraction {
		class NetConnection {
		public:
			void* pointer_to_torque_netconnection_class_ = NULL;
			NetConnection(void* pointer_to_torque_netconnection_class);
		};

		namespace hooks {
			namespace NetConnection {
				void __fastcall StartDemoRecordHook(void*, void*, const char*);
				typedef void(__thiscall* StartDemoRecord)(void*, const char*);
				extern StartDemoRecord OriginalStartDemoRecord;


				void __fastcall StopDemoRecordHook(void*, void*);
				typedef void(__thiscall* StopDemoRecord)(void*);
				extern StopDemoRecord OriginalStopDemoRecord;
			}
		}
	}
}
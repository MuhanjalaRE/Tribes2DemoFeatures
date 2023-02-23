#include <t2/abstraction/NetConnection.h>
#include <t2/game data/demo.h>

namespace t2 {
	namespace abstraction {
		namespace hooks {
			namespace NetConnection {

				StartDemoRecord OriginalStartDemoRecord = (StartDemoRecord)0x00587C30;
				void __fastcall StartDemoRecordHook(void* this_netconnection, void* _, const char* filename) {
					OriginalStartDemoRecord(this_netconnection, filename);
					return;
				}

				void __fastcall StopDemoRecordHook(void* this_netconnection, void* _) {
					OriginalStopDemoRecord(this_netconnection);
					return;
				}
				StopDemoRecord OriginalStopDemoRecord = (StopDemoRecord)0x00587F20;
			}
		}
	}
}
#include <t2/abstraction/Camera.h>

namespace t2 {
	namespace abstraction {
		namespace hooks {
			namespace Camera {
				SetPosition OriginalSetPosition = (SetPosition)0x5CC270;

				ProcessTick OriginalProcessTick = (ProcessTick)0x5CBC80;
				void __fastcall ProcessTickHook(void* this_camera, void* _, void* move) {
					return;
				}

				SetFlyMode OriginalSetFlyMode = (SetFlyMode)0x005CCCE0;
			}
		}
	}
}
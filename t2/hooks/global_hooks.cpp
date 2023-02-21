#include "global_hooks.h"
#include <t2/settings/settings.h>

namespace t2 {
	namespace hooks {
		namespace con {
			Printf OriginalPrintf = (Printf)0x00425F30;
			int __stdcall PrintfHook(char* format, ...) {
				va_list argptr;
				va_start(argptr, format);
				int ret = vfprintf(stdout, format, argptr);
				fprintf(stdout, "\n");
				va_end(argptr);
				return ret;
			}
		}

		namespace fps {
			FpsUpdate OriginalFpsUpdate = (FpsUpdate)0x00564570;
			void FpsUpdateHook(void) {
				t2::hooks::game::OriginalSetCameraFOV(t2::settings::camera_fov);
				OriginalFpsUpdate();
			}
		}

		namespace game {
			SetCameraFOV OriginalSetCameraFOV = (SetCameraFOV)0x005BB080;
			void __cdecl SetCameraFOVHook(float fov) {
				return;
			}
		}
	}
}
#pragma once

//#include <t2/abstraction/SimObject.h>
#include <t2/math.h>
#include <t2/abstraction/SceneObject.h>

namespace t2 {
	namespace hooks {
		namespace con {
			int __stdcall PrintfHook(char* format, ...);
			typedef int(__stdcall* Printf)(char* format, ...);
			extern Printf OriginalPrintf;
		}

		namespace fps {
			typedef void(__cdecl* FpsUpdate)(void);
			extern FpsUpdate OriginalFpsUpdate;
			void FpsUpdateHook(void);
		}

		namespace game {
			typedef void(__cdecl* SetCameraFOV)(float fov);
			extern SetCameraFOV OriginalSetCameraFOV;
			void __cdecl SetCameraFOVHook(float fov);
		}
	}
}
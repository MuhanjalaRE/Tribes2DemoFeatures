#pragma once

//#include <t2/abstraction/SimObject.h>
#include <t2/math.h>
#include <t2/abstraction/SceneObject.h>
#include <Windows.h>
#include <imgui/imgui.h>

namespace t2 {
	namespace hooks {
		namespace con {
			int __stdcall PrintfHook(char* format, ...);
			typedef int(__stdcall* Printf)(char* format, ...);
			extern Printf OriginalPrintf;

			typedef const char* (__cdecl* Executef)(int argc, ...);
			extern Executef OriginalExecutef;
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
			

			typedef bool(__cdecl* GameProcessCameraQuery)(void*);
			extern GameProcessCameraQuery OriginalGameProcessCameraQuery;
			bool __cdecl GameProcessCameraQueryHook(void*);
		}

		namespace platform {
			typedef void (__cdecl* SetWindowLocked)(bool);
			extern SetWindowLocked OriginalSetWindowLocked;
			void __cdecl SetWindowLockedHook(bool locked);
		}

		namespace opengl {
			extern std::vector<ImVec2> projection_buffer;
			extern HANDLE game_mutex;
			typedef int(__stdcall* GluProject)(double objx, double objy, double objz, const double modelMatrix[16], const double projMatrix[16], const int viewport[4], double* winx, double* winy, double* winz);
			extern GluProject OriginalGluProject;
			int __stdcall GluProjectHook(double objx, double objy, double objz, const double modelMatrix[16], const double projMatrix[16], const int viewport[4], double* winx, double* winy, double* winz);
		}

		namespace guicanvas {
			typedef void(__thiscall* RenderFrame)(void*, bool);
			extern RenderFrame OriginalRenderFrame;
			void __fastcall RenderFrameHook(void*, void*, bool);
		}

		namespace guicontrol {
			typedef void(__thiscall* OnRender)(void*, void*, void*, void*);
			extern OnRender OriginalOnRender;
			void __fastcall OnRenderHook(void*, void*, void*, void*, void*);
			extern OnRender OriginalOnRender2;
			void __fastcall OnRenderHook2(void*, void*, void*, void*, void*);
			typedef void(__thiscall* RenderChildControls)(void*, void*, void*, void*);
			extern RenderChildControls OriginalRenderChildControls;
			void __fastcall RenderChildControlsHook(void*, void*, void*, void*, void*);

			typedef char* (__thiscall* sub_505740)(void*, void*, void*, void*);
			extern sub_505740 Originalsub_505740;
			char* __fastcall sub_505740Hook(void*, void*, void*, void*, void*);

			typedef char(__thiscall* sub_506870)(void*, void*, void*, void*);
			extern sub_506870 Originalsub_506870;
			char __fastcall sub_506870Hook(void*, void*, void*, void*, void*);

			typedef int(__thiscall* sub_5046A0)(void*, void*, void*, void*, void*, void*);
			extern sub_5046A0 Originalsub_5046A0;
			int(__fastcall sub_5046A0Hook)(void*, void*, void*, void*, void*, void*, void*);
		}

		namespace wintimer {
			typedef void(__stdcall* GetElapsedMS)(void);
			extern GetElapsedMS OriginalGetElapsedMS;
			void __stdcall GetElapsedMSHook(void);
		}
	}
}